/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "powsuppsolve.h"

PowSuppSolve::PowSuppSolve(QObject *parent)
    :QObject(parent)
{
    m_bc.reset(new BCap);
    m_db.reset(new DBridge);
    m_pm.reset(new PMosfet);
    m_ptpe.reset(new PulseTransPrimaryElectr());
    m_ptsw.reset(new PulseTransWires());
    m_fod.reset(new FullOutDiode);
    m_foc.reset(new FullOutCap);
    m_of.reset(new FullOutFilter);
    m_pssm.reset(new PowerStageSmallSignalModel);
    m_ofs.reset(new OptocouplerFedbackStage);

    m_isSolveRunning = false;
    m_isSolveAbort = false;
}

PowSuppSolve::~PowSuppSolve()
{}

void PowSuppSolve::requestCalc()
{
    QMutexLocker locker(&m_mutex);
    m_isSolveRunning = true;
    m_isSolveAbort = false;
    qDebug() << "Request calculate start in Thread " << thread()->currentThreadId();
    emit calcRequested();
}

void PowSuppSolve::abort()
{
    QMutexLocker locker(&m_mutex);
    if(m_isSolveRunning){
        m_isSolveAbort = true;
        qDebug() << "Request calculate aborting in Thread " << thread()->currentThreadId();
    }
}

void PowSuppSolve::calcInputNetwork()
{
    emit startCalcInputNetwork();
    qDebug() << "Start calculate input network in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    QScopedPointer<BulkCap> b_cap(new BulkCap(m_indata.input_volt_ac_max,
                                              m_indata.input_volt_ac_min,
                                              static_cast<float>(m_indata.eff),
                                              static_cast<float>(m_indata.power_out_max),
                                              m_indata.freq_line));
    qDebug() << "Initialize BulkCap object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with BulkCap in Thread " << thread()->currentThreadId();

    /**< Fill the structure */
    m_bc->delta_t = b_cap->DeltaT();
    m_bc->charg_time = b_cap->ChargTime();
    m_bc->bcapacitor_value = b_cap->CapValue();
    m_bc->load_curr_max = b_cap->ILoadMax();
    m_bc->load_curr_min = b_cap->ILoadMin();
    m_bc->bcapacitor_peak_curr = b_cap->IBulkCapPeak();
    m_bc->input_min_voltage = b_cap->VMinInp();
    m_bc->input_dc_min_voltage = b_cap->VDCMin();

    QScopedPointer<DiodeBridge> b_diode(new DiodeBridge(m_indata.input_volt_ac_max,
                                                        m_indata.input_volt_ac_min,
                                                        static_cast<float>(m_indata.eff),
                                                        static_cast<float>(m_indata.power_out_max),
                                                        m_indata.freq_line));
    qDebug() << "Initialize DiodeBridge object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with DiodeBridge in Thread " << thread()->currentThreadId();

    /**< 1. Set capacitor params */
    b_diode->setBcapParam(static_cast<float>(m_bc->bcapacitor_peak_curr) , m_bc->charg_time);
    /**< 2. Fill the structure */
    m_db->diode_peak_curr = b_diode->IDiodePeak();
    m_db->diode_curr_slope = b_diode->DiodeCurrentSlope();
    m_db->diode_cond_time = b_diode->DiodeConductTime();
    m_db->load_avg_curr = b_diode->ILoadAVG();
    m_db->diode_avg_curr = b_diode->IDiodeAVG();
    m_db->diode_rms_curr = b_diode->IDiodeRMS();
    m_db->diode_rms_curr_tot = b_diode->IDiodeRMSTot();
    m_db->in_min_rms_voltage = b_diode->MinPeakInVoltage();
    m_db->in_max_rms_voltage = b_diode->MaxPeakInVoltage();
    /**< 3. Set capacitor RMS current */
    m_bc->bcapacitor_rms_curr = b_cap->IBulkCapRMS(m_db->diode_avg_curr, m_db->diode_cond_time);

    emit finishedInputNetwork();
    qDebug() << "Finished calculate input network in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcElectricalPrimarySide()
{
    emit startCalcElectricalPrimarySide();
    qDebug() << "Start calculate electrical primary side in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    QScopedPointer<FBPTPrimary> t_prim(new FBPTPrimary(static_cast<double>(m_indata.ripple_fact),
                                                       m_indata.refl_volt_max,
                                                       m_indata.power_out_max,
                                                       static_cast<float>(m_indata.eff),
                                                       m_indata.freq_switch));
    qDebug() << "Initialize FBPTPrimary object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with FBPTPrimary in Thread " << thread()->currentThreadId();

    /**< 1. Set input voltage */
    m_ptpe->inp_power = t_prim->InputPower();
    t_prim->setInputVoltage(m_indata.input_volt_ac_min,
                            m_ptpe->inp_power,
                            m_indata.freq_line,
                            m_bc->bcapacitor_value,
                            m_bc->delta_t);
    /**< 2. Fill the structure for primary side */
    m_ptpe->max_duty_cycle = t_prim->DutyCycleDCM();
    m_ptpe->primary_induct = t_prim->PriInduct();
    m_ptpe->curr_primary_aver = t_prim->CurrPriAver();
    m_ptpe->curr_primary_peak_peak = t_prim->CurrPriPeakToPeak();
    m_ptpe->curr_primary_peak = t_prim->CurrPriMax();
    m_ptpe->curr_primary_valley = t_prim->CurrPriValley();
    m_ptpe->curr_primary_rms = t_prim->CurrPriRMS();

    emit finishedCalcElectricalPrimarySide();
    qDebug() << "Finished calculate electrical primary side in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcArea()
{
    emit startCalcArea();
    qDebug() << "Start calculate area in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    m_core.reset(new FBPTCore(m_ca, m_ptpe->primary_induct,
                              m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms,
                              m_ptpe->curr_primary_peak_peak, m_indata.power_out_max));

    qDebug() << "Initialize FBPTCore object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with FBPTCore in Thread " << thread()->currentThreadId();

    m_ptpe->core_area_product = m_core->CoreAreaProd();
    m_ptpe->core_geom_coeff = m_core->CoreGeometryCoeff(m_indata.power_out_max);

    emit finishedCalcArea();
    qDebug() << "Finished calculate area in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcElectroMagProperties()
{
    emit startCalcElectroMagProperties();
    qDebug() << "Start calculate magnetic properties in Thread " << thread()->currentThreadId();

    /*if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    QVector<QPair<float, float>> out_vlcr;
    out_vlcr.reserve(4);

    auto ofrst = qMakePair(static_cast<float>(m_indata.volt_out_one),
                           static_cast<float>(m_indata.curr_out_one));
    out_vlcr.push_back(ofrst);
    auto osec = qMakePair(static_cast<float>(m_indata.volt_out_two),
                          static_cast<float>(m_indata.curr_out_two));
    out_vlcr.push_back(osec);
    auto othir = qMakePair(static_cast<float>(m_indata.volt_out_three),
                            static_cast<float>(m_indata.curr_out_three));
    out_vlcr.push_back(othir);
    auto ofour = qMakePair(static_cast<float>(m_indata.volt_out_four),
                           static_cast<float>(m_indata.curr_out_four));
    out_vlcr.push_back(ofour);
    qDebug() << "Initialize vector with capacity: " << out_vlcr.capacity() << " size: " << out_vlcr.size() << " value [0]: " << out_vlcr[0].first;
    qDebug() << "Initialize vector with capacity: " << out_vlcr.capacity() << " size: " << out_vlcr.size() << " value [1]: " << out_vlcr[1].first;
    qDebug() << "Initialize vector with capacity: " << out_vlcr.capacity() << " size: " << out_vlcr.size() << " value [2]: " << out_vlcr[2].first;
    qDebug() << "Initialize vector with capacity: " << out_vlcr.capacity() << " size: " << out_vlcr.size() << " value [3]: " << out_vlcr[3].first;

    qDebug() << "Write electromagnetic properties in Thread " << thread()->currentThreadId();

    m_ptpe->curr_dens = m_core->CurrentDens(m_cs);
    m_ptpe->number_primary = static_cast<uint32_t>(m_core->numPrimary(m_cs, m_fns));
    m_ptpe->length_air_gap = m_core->agLength(m_cs, m_ptpe->number_primary);
    m_ptpe->fring_flux_fact = m_core->agFringFluxFact(m_cs, m_ptpe->number_primary, m_fsag, m_md);

    m_ptpe->actual_num_primary = static_cast<uint32_t>(m_core->actNumPrimary(m_cs,
                                                       m_fsag,
                                                       m_md,
                                                       m_ptpe->number_primary,
                                                       m_ptpe->primary_induct,
                                                       m_ptpe->curr_primary_peak));

    m_ptpe->actual_flux_dens_peak = m_core->actMagneticFluxPeak(m_cs,
                                                                m_ptpe->actual_num_primary,
                                                                m_ptpe->curr_primary_peak,
                                                                m_ptpe->length_air_gap);

    m_ptpe->actual_max_duty_cycle = m_core->actDutyCycle(out_vlcr,
                                                         m_bc->input_dc_min_voltage,
                                                         m_indata.freq_switch,
                                                         m_ptpe->primary_induct);

    m_ptpe->actual_volt_reflected = m_core->actReflVoltage(static_cast<float>(m_ptpe->actual_max_duty_cycle),
                                                           static_cast<float>(m_indata.power_out_max),
                                                           m_ptpe->primary_induct,
                                                           m_indata.freq_switch);
    emit finishedCalcElectroMagProperties();
    qDebug() << "Finished calculate magnetic properties in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcTransformerWired()
{
    emit startCalcTransformerWired();
    qDebug() << "Start calculate transformer wired in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/
    // Make secondary side objects
    QSharedPointer<FBPTSecondary> sec_one = QSharedPointer<FBPTSecondary>(new FBPTSecondary(m_indata.curr_out_one,
                                                                                            m_indata.volt_out_one,
                                                                                            static_cast<float>(m_ptpe->actual_volt_reflected),
                                                                                            static_cast<float>(m_indata.power_out_max),
                                                                                            static_cast<int16_t>(m_ptpe->actual_num_primary),
                                                                                            static_cast<float>(m_ptpe->actual_max_duty_cycle),
                                                                                            m_indata.volt_diode_drop_sec));

    QSharedPointer<FBPTSecondary> sec_two = QSharedPointer<FBPTSecondary>(new FBPTSecondary(m_indata.curr_out_two,
                                                                                            m_indata.volt_out_two,
                                                                                            static_cast<float>(m_ptpe->actual_volt_reflected),
                                                                                            static_cast<float>(m_indata.power_out_max),
                                                                                            static_cast<int16_t>(m_ptpe->actual_num_primary),
                                                                                            static_cast<float>(m_ptpe->actual_max_duty_cycle),
                                                                                            m_indata.volt_diode_drop_sec));

    QSharedPointer<FBPTSecondary> sec_three = QSharedPointer<FBPTSecondary>(new FBPTSecondary(m_indata.curr_out_three,
                                                                                              m_indata.volt_out_three,
                                                                                              static_cast<float>(m_ptpe->actual_volt_reflected),
                                                                                              static_cast<float>(m_indata.power_out_max),
                                                                                              static_cast<int16_t>(m_ptpe->actual_num_primary),
                                                                                              static_cast<float>(m_ptpe->actual_max_duty_cycle),
                                                                                              m_indata.volt_diode_drop_sec));

    QSharedPointer<FBPTSecondary> sec_four = QSharedPointer<FBPTSecondary>(new FBPTSecondary(m_indata.curr_out_four,
                                                                                             m_indata.volt_out_four,
                                                                                             static_cast<float>(m_ptpe->actual_volt_reflected),
                                                                                             static_cast<float>(m_indata.power_out_max),
                                                                                             static_cast<int16_t>(m_ptpe->actual_num_primary),
                                                                                             static_cast<float>(m_ptpe->actual_max_duty_cycle),
                                                                                             m_indata.volt_diode_drop_sec));

    QSharedPointer<FBPTSecondary> aux_out = QSharedPointer<FBPTSecondary>(new FBPTSecondary(m_indata.curr_out_aux,
                                                                                            m_indata.volt_out_aux,
                                                                                            static_cast<float>(m_ptpe->actual_volt_reflected),
                                                                                            static_cast<float>(m_indata.power_out_max),
                                                                                            static_cast<int16_t>(m_ptpe->actual_num_primary),
                                                                                            static_cast<float>(m_ptpe->actual_max_duty_cycle),
                                                                                            m_indata.volt_diode_drop_sec));

    qDebug() << "Make secondary side objects in Thread " << thread()->currentThreadId();

    // Packing of the secondary side objects
    m_sec.push_back(sec_one);
    m_sec.push_back(sec_two);
    m_sec.push_back(sec_three);
    m_sec.push_back(sec_four);

    qDebug() << "Packing of the secondary side objects in Thread " << thread()->currentThreadId();


    // Make winding objects
    QSharedPointer<FBPTWinding> wind_prim = QSharedPointer<FBPTWinding>(new FBPTWinding(m_indata.freq_switch,
                                                                                        m_psw.m_mcd,
                                                                                        static_cast<double>(m_psw.m_fcu),
                                                                                        static_cast<double>(m_psw.m_ins[0])));

    QSharedPointer<FBPTWinding> wind_sec_one = QSharedPointer<FBPTWinding>(new FBPTWinding(m_indata.freq_switch,
                                                                                           m_psw.m_mcd,
                                                                                           static_cast<double>(m_psw.m_fcu),
                                                                                           static_cast<double>(m_psw.m_ins[1])));

    QSharedPointer<FBPTWinding> wind_sec_two = QSharedPointer<FBPTWinding>(new FBPTWinding(m_indata.freq_switch,
                                                                                           m_psw.m_mcd,
                                                                                           static_cast<double>(m_psw.m_fcu),
                                                                                           static_cast<double>(m_psw.m_ins[2])));

    QSharedPointer<FBPTWinding> wind_sec_three = QSharedPointer<FBPTWinding>(new FBPTWinding(m_indata.freq_switch,
                                                                                             m_psw.m_mcd,
                                                                                             static_cast<double>(m_psw.m_fcu),
                                                                                             static_cast<double>(m_psw.m_ins[3])));

    QSharedPointer<FBPTWinding> wind_sec_four = QSharedPointer<FBPTWinding>(new FBPTWinding(m_indata.freq_switch,
                                                                                            m_psw.m_mcd,
                                                                                            static_cast<double>(m_psw.m_fcu),
                                                                                            static_cast<double>(m_psw.m_ins[4])));

    QSharedPointer<FBPTWinding> wind_aux = QSharedPointer<FBPTWinding>(new FBPTWinding(m_indata.freq_switch,
                                                                                       m_psw.m_mcd,
                                                                                       static_cast<double>(m_psw.m_fcu),
                                                                                       static_cast<double>(m_psw.m_ins[5])));

    qDebug() << "Make winding objects in Thread " << thread()->currentThreadId();

    // Packing of the winding objects in to sequence container
    m_wind.push_back(wind_prim);
    m_wind.push_back(wind_sec_one);
    m_wind.push_back(wind_sec_two);
    m_wind.push_back(wind_sec_three);
    m_wind.push_back(wind_sec_four);
    m_wind.push_back(wind_aux);

    qDebug() << "Packing of the winding objects in to sequence container in Thread " << thread()->currentThreadId();

    // Packing winding properties of the primary side
    m_ptsw->primary_wind.insert("AP", static_cast<float>(m_wind[0]->wCoperWireCrossSectArea(m_cs,
                                                                                            m_md,
                                                                                            static_cast<double>(m_psw.m_af[0]),
                                                                                            m_ptpe->actual_num_primary)));

    m_ptsw->primary_wind.insert("AWGP", static_cast<float>(m_wind[0]->wMaxWireSizeAWG(static_cast<double>(m_ptsw->primary_wind.value("AP")))));

    m_wind[0]->setWireDiam(m_ptsw->primary_wind.value("AWGP"));

    m_ptsw->primary_wind.insert("DP", static_cast<float>(m_wind[0]->wCoperWireDiam()));
    m_ptsw->primary_wind.insert("ECA", static_cast<float>(m_wind[0]->wCoperWireCrossSectAreaPost(m_psw.m_npw[0])));
    m_ptsw->primary_wind.insert("JP", static_cast<float>(m_wind[0]->wCurrentDenst(m_ptpe->curr_primary_rms, m_psw.m_npw[0])));
    m_ptsw->primary_wind.insert("OD", static_cast<float>(m_wind[0]->wOuterDiam()));
    m_ptsw->primary_wind.insert("NTL", static_cast<float>(m_wind[0]->wNumTurnToLay(m_md, m_psw.m_npw[0])));
    m_ptsw->primary_wind.insert("LN", static_cast<float>( m_wind[0]->wNumLay(m_md, m_ptpe->actual_num_primary, m_psw.m_npw[0])));

    // Packing winding properties of the 1st secondary side
    m_sec[0]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_one_wind.insert("JSP", static_cast<float>(m_sec[0]->outCurrPeakSecond()));
    m_ptsw->out_one_wind.insert("JSRMS", static_cast<float>(m_sec[0]->outCurrRMSSecond()));
    m_ptsw->out_one_wind.insert("NSEC", static_cast<float>(m_sec[0]->outNumSecond()));
    m_ptsw->out_one_wind.insert("ANS", static_cast<float>(m_wind[1]->wCoperWireCrossSectArea(m_cs,
                                                                                             m_md,
                                                                                             static_cast<double>(m_psw.m_af[1]),
                                                                                             static_cast<uint32_t>(m_ptsw->out_one_wind.value("NSEC")))));

    m_ptsw->out_one_wind.insert("AWGNS", static_cast<float>(m_wind[1]->wMaxWireSizeAWG(static_cast<double>(m_ptsw->out_one_wind.value("ANS")))));

    m_wind[1]->setWireDiam(m_ptsw->out_one_wind.value("AWGNS"));

    m_ptsw->out_one_wind.insert("DS", static_cast<float>(m_wind[1]->wCoperWireDiam()));
    m_ptsw->out_one_wind.insert("ECA", static_cast<float>(m_wind[1]->wCoperWireCrossSectAreaPost(m_psw.m_npw[1])));
    m_ptsw->out_one_wind.insert("JS", static_cast<float>(m_wind[1]->wCurrentDenst(static_cast<double>(m_ptsw->out_one_wind.value("JSRMS")), m_psw.m_npw[1])));
    m_ptsw->out_one_wind.insert("OD", static_cast<float>(m_wind[1]->wOuterDiam()));
    m_ptsw->out_one_wind.insert("NTL", static_cast<float>(m_wind[1]->wNumTurnToLay(m_md, m_psw.m_npw[1])));
    m_ptsw->out_one_wind.insert("LN", static_cast<float>(m_wind[1]->wNumLay(m_md, static_cast<uint32_t>(m_ptsw->out_one_wind.value("NSEC")), m_psw.m_npw[1])));

    // Packing winding properties of the 2nd secondary side
    m_sec[1]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_two_wind.insert("JSP", static_cast<float>(m_sec[1]->outCurrPeakSecond()));
    m_ptsw->out_two_wind.insert("JSRMS", static_cast<float>(m_sec[1]->outCurrRMSSecond()));
    m_ptsw->out_two_wind.insert("NSEC", static_cast<float>(m_sec[1]->outNumSecond()));
    m_ptsw->out_two_wind.insert("ANS", static_cast<float>(m_wind[2]->wCoperWireCrossSectArea(m_cs,
                                                                                             m_md,
                                                                                             static_cast<double>(m_psw.m_af[2]),
                                                                                             static_cast<uint32_t>(m_ptsw->out_two_wind.value("NSEC")))));

    m_ptsw->out_two_wind.insert("AWGNS", static_cast<float>(m_wind[2]->wMaxWireSizeAWG(static_cast<double>(m_ptsw->out_two_wind.value("ANS")))));

    m_wind[2]->setWireDiam(m_ptsw->out_two_wind.value("AWGNS"));

    m_ptsw->out_two_wind.insert("DS", static_cast<float>(m_wind[2]->wCoperWireDiam()));
    m_ptsw->out_two_wind.insert("ECA", static_cast<float>(m_wind[2]->wCoperWireCrossSectAreaPost(m_psw.m_npw[2])));
    m_ptsw->out_two_wind.insert("JS", static_cast<float>(m_wind[2]->wCurrentDenst(static_cast<double>(m_ptsw->out_two_wind.value("JSRMS")), m_psw.m_npw[2])));
    m_ptsw->out_two_wind.insert("OD", static_cast<float>(m_wind[2]->wOuterDiam()));
    m_ptsw->out_two_wind.insert("NTL", static_cast<float>(m_wind[2]->wNumTurnToLay(m_md, m_psw.m_npw[2])));
    m_ptsw->out_two_wind.insert("LN", static_cast<float>(m_wind[2]->wNumLay(m_md, static_cast<uint32_t>(m_ptsw->out_two_wind.value("NSEC")), m_psw.m_npw[2])));

    // Packing winding properties of the 3th secondary side
    m_sec[2]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_three_wind.insert("JSP", static_cast<float>(m_sec[2]->outCurrPeakSecond()));
    m_ptsw->out_three_wind.insert("JSRMS", static_cast<float>(m_sec[2]->outCurrRMSSecond()));
    m_ptsw->out_three_wind.insert("NSEC", static_cast<float>(m_sec[2]->outNumSecond()));
    m_ptsw->out_three_wind.insert("ANS", static_cast<float>(m_wind[3]->wCoperWireCrossSectArea(m_cs,
                                                                                               m_md,
                                                                                               static_cast<double>(m_psw.m_af[3]),
                                                                                               static_cast<uint32_t>(m_ptsw->out_three_wind.value("NSEC")))));

    m_ptsw->out_three_wind.insert("AWGNS", static_cast<float>(m_wind[3]->wMaxWireSizeAWG(static_cast<double>(m_ptsw->out_three_wind.value("ANS")))));

    m_wind[3]->setWireDiam(m_ptsw->out_three_wind.value("AWGNS"));

    m_ptsw->out_three_wind.insert("DS", static_cast<float>(m_wind[3]->wCoperWireDiam()));
    m_ptsw->out_three_wind.insert("ECA", static_cast<float>(m_wind[3]->wCoperWireCrossSectAreaPost(m_psw.m_npw[3])));
    m_ptsw->out_three_wind.insert("JS", static_cast<float>(m_wind[3]->wCurrentDenst(static_cast<double>(m_ptsw->out_three_wind.value("JSRMS")), m_psw.m_npw[3])));
    m_ptsw->out_three_wind.insert("OD", static_cast<float>(m_wind[3]->wOuterDiam()));
    m_ptsw->out_three_wind.insert("NTL", static_cast<float>(m_wind[3]->wNumTurnToLay(m_md,m_psw.m_npw[3])));
    m_ptsw->out_three_wind.insert("LN", static_cast<float>(m_wind[3]->wNumLay(m_md, static_cast<uint32_t>(m_ptsw->out_three_wind.value("NSEC")), m_psw.m_npw[3])));

    // Packing winding properties of the 4th secondary side
    m_sec[3]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_four_wind.insert("JSP", static_cast<float>(m_sec[3]->outCurrPeakSecond()));
    m_ptsw->out_four_wind.insert("JSRMS", static_cast<float>(m_sec[3]->outCurrRMSSecond()));
    m_ptsw->out_four_wind.insert("NSEC", static_cast<float>(m_sec[3]->outNumSecond()));
    m_ptsw->out_four_wind.insert("ANS", static_cast<float>(m_wind[4]->wCoperWireCrossSectArea(m_cs,
                                                                                              m_md,
                                                                                              static_cast<double>(m_psw.m_af[4]),
                                                                                              static_cast<uint32_t>(m_ptsw->out_four_wind.value("NSEC")))));

    m_ptsw->out_four_wind.insert("AWGNS", static_cast<float>(m_wind[4]->wMaxWireSizeAWG(static_cast<double>(m_ptsw->out_four_wind.value("ANS")))));

    m_wind[4]->setWireDiam(m_ptsw->out_four_wind.value("AWGNS"));

    m_ptsw->out_four_wind.insert("DS", static_cast<float>(m_wind[4]->wCoperWireDiam()));
    m_ptsw->out_four_wind.insert("ECA", static_cast<float>(m_wind[4]->wCoperWireCrossSectAreaPost(m_psw.m_npw[4])));
    m_ptsw->out_four_wind.insert("JS", static_cast<float>(m_wind[4]->wCurrentDenst(static_cast<double>(m_ptsw->out_four_wind.value("JSRMS")), m_psw.m_npw[4])));
    m_ptsw->out_four_wind.insert("OD", static_cast<float>(m_wind[4]->wOuterDiam()));
    m_ptsw->out_four_wind.insert("NTL", static_cast<float>(m_wind[4]->wNumTurnToLay(m_md, m_psw.m_npw[4])));
    m_ptsw->out_four_wind.insert("LN", static_cast<float>(m_wind[4]->wNumLay(m_md, static_cast<uint32_t>(m_ptsw->out_four_wind.value("NSEC")), m_psw.m_npw[4])));

    // Packing winding properties of the auxilary side
    m_ptsw->out_aux_wind.insert("NAUX", static_cast<float>(aux_out->outNumSecond()));
    m_ptsw->out_aux_wind.insert("ANAUX", static_cast<float>(m_wind[5]->wCoperWireCrossSectArea(m_cs,
                                                                                               m_md,
                                                                                               static_cast<double>(m_psw.m_af[5]),
                                                                                               static_cast<uint32_t>(m_ptsw->out_aux_wind.value("NAUX")))));

    m_ptsw->out_aux_wind.insert("AWGAUX", static_cast<float>(m_wind[5]->wMaxWireSizeAWG(static_cast<double>(m_ptsw->out_aux_wind.value("ANAUX")))));

    m_wind[5]->setWireDiam(m_ptsw->out_aux_wind.value("AWGAUX"));

    m_ptsw->out_aux_wind.insert("DAUX", static_cast<float>(m_wind[5]->wCoperWireDiam()));
    m_ptsw->out_aux_wind.insert("ECA", static_cast<float>(m_wind[5]->wCoperWireCrossSectAreaPost(m_psw.m_npw[5])));
    m_ptsw->out_aux_wind.insert("OD", static_cast<float>(m_wind[5]->wOuterDiam()));
    m_ptsw->out_aux_wind.insert("NTL", static_cast<float>(m_wind[5]->wNumTurnToLay(m_md, m_psw.m_npw[5])));

    qDebug() << "Packing winding properties objects in Thread " << thread()->currentThreadId();

    emit finishedCalcTransformerWired();
    qDebug() << "Finished calculate transformer wired in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcSwitchNetwork()
{
    emit startCalcSwitchNetwork();
    qDebug() << "Start calculate switch network in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    auto vmaxrms = static_cast<uint16_t>(m_indata.input_volt_ac_max * M_SQRT2);
    //int16_t vminrms = static_cast<int16_t>(m_indata.input_volt_ac_min * M_SQRT2);

    QScopedPointer<SwMosfet> sw_mos(new SwMosfet(vmaxrms,
                                                 m_indata.voltage_spike,
                                                 m_indata.freq_switch,
                                                 m_ptpe->actual_volt_reflected,
                                                 m_ptpe->primary_induct,
                                                 m_ptpe->curr_primary_peak_peak));

    qDebug() << "Initialise switching mosfet object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with switching mosfet in Thread " << thread()->currentThreadId();

    m_pm->mosfet_voltage_nom = static_cast<int16_t>(sw_mos->swMosfetVoltageNom());
    m_pm->mosfet_voltage_max = static_cast<int16_t>(sw_mos->swMosfetVoltageMax());
    sw_mos->setCurrValues(static_cast<float>(m_ptpe->curr_primary_rms), static_cast<float>(m_ptpe->curr_primary_peak));
    m_pm->mosfet_ds_curr = static_cast<float>(m_ptpe->curr_primary_peak);
    m_pm->mosfet_on_time = 0.;
    m_pm->mosfet_off_time = 0.;
    m_pm->mosfet_fall_time = sw_mos->swMosfetFallTime(m_mospr);
    m_pm->mosfet_rise_time = sw_mos->swMosfetRiseTime(m_mospr);
    m_pm->mosfet_conduct_loss = static_cast<float>(sw_mos->swMosfetConductLoss(m_mospr));
    m_pm->mosfet_drive_loss = static_cast<float>(sw_mos->swMosfetDriveLoss(m_mospr));
    m_pm->mosfet_switch_loss = static_cast<float>(sw_mos->swMosfetSwitchLoss(m_mospr));
    m_pm->mosfet_capacit_loss = static_cast<float>(sw_mos->swMosfetCapacitLoss(m_mospr));
    m_pm->mosfet_total_loss = static_cast<float>(sw_mos->swMosfetTotalLoss(m_mospr));

    m_pm->snubber_voltage_max = static_cast<int32_t>(sw_mos->clVoltageMax());
    m_pm->snubber_cap_value = sw_mos->clCapValue(m_ccsp);
    m_pm->snubber_res_value = static_cast<int32_t>(sw_mos->clResValue(m_ccsp));
    m_pm->snubber_pwr_diss = static_cast<float>(sw_mos->clPowerDiss(m_ccsp));

    m_pm->curr_sense_res = static_cast<float>(sw_mos->csCurrRes(m_ccsp));
    m_pm->curr_sense_res_loss = static_cast<float>(sw_mos->csCurrResLoss(m_ccsp));

    emit finishedCalcSwitchNetwork();
    qDebug() << "Finished calculate switch network in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcOtputNetwork()
{
    emit startCalcOtputNetwork();
    qDebug() << "Start calculate output network in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }
    */

    auto turnRatio = [=](uint32_t num_pr, uint32_t num_sec, bool volt_rt = true)
    {
        if((num_pr != 0)&&(num_sec != 0))
        {

            if(volt_rt)
            {
                return static_cast<double>(num_pr) / num_sec;
            }
            else
            {
                return static_cast<double>(num_sec) / num_pr;
            }
        }
        else
        {
            return  0.;
        }
    };

    auto outPwr = [=](float out_volt, int16_t out_curr)
    {
        return out_volt * out_curr;
    };


    //Construct output diode objects
    QScopedPointer<DiodeOut> d_out_one(new DiodeOut(outPwr(m_indata.curr_out_one, m_indata.volt_out_one),
                                                    m_indata.volt_out_one,
                                                    turnRatio(m_ptpe->actual_num_primary, m_ptsw->out_one_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_two(new DiodeOut(outPwr(m_indata.curr_out_two, m_indata.volt_out_two),
                                                    m_indata.volt_out_two,
                                                    turnRatio(m_ptpe->actual_num_primary, m_ptsw->out_two_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_three(new DiodeOut(outPwr(m_indata.curr_out_three, m_indata.volt_out_three),
                                                      m_indata.volt_out_three,
                                                      turnRatio(m_ptpe->actual_num_primary, m_ptsw->out_three_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_four(new DiodeOut(outPwr(m_indata.curr_out_four, m_indata.volt_out_four),
                                                     m_indata.volt_out_four,
                                                     turnRatio(m_ptpe->actual_num_primary, m_ptsw->out_four_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_aux(new DiodeOut(outPwr(m_indata.curr_out_aux, m_indata.volt_out_aux),
                                                    m_indata.volt_out_aux,
                                                    turnRatio(m_ptpe->actual_num_primary, m_ptsw->out_aux_wind.value("NAUX"))));
    qDebug() << "Start calculate output diode in Thread " << thread()->currentThreadId();

    //Construct output capacitor objects
    QScopedPointer<CapOut> c_out_one(new CapOut(m_cop[0]));
    QScopedPointer<CapOut> c_out_two(new CapOut(m_cop[1]));
    QScopedPointer<CapOut> c_out_three(new CapOut(m_cop[2]));
    QScopedPointer<CapOut> c_out_four(new CapOut(m_cop[3]));
    QScopedPointer<CapOut> c_out_aux(new CapOut(m_cop[4]));

    qDebug() << "Start calculate output capacitor in Thread " << thread()->currentThreadId();

    //Packing output diode values
    m_fod->out_diode_first.insert("SOP", outPwr(m_indata.curr_out_one,
                                                m_indata.volt_out_one));
    m_fod->out_diode_first.insert("SOV", m_indata.volt_out_one);
    m_fod->out_diode_first.insert("TR", static_cast<float>(turnRatio(m_ptpe->actual_num_primary,
                                                                     static_cast<uint32_t>(m_ptsw->out_one_wind.value("NSEC")))));
    m_fod->out_diode_first.insert("DRV", static_cast<float>(d_out_one->doDiodeRevVolt(m_indata.input_volt_ac_max)));
    m_fod->out_diode_first.insert("DPD", static_cast<float>(d_out_aux->doDiodePowLoss(m_indata.volt_diode_drop_sec)));
    //
    m_fod->out_diode_sec.insert("SOP", outPwr(m_indata.curr_out_two,
                                              m_indata.volt_out_two));
    m_fod->out_diode_sec.insert("SOV", m_indata.volt_out_two);
    m_fod->out_diode_sec.insert("TR", static_cast<float>(turnRatio(m_ptpe->actual_num_primary,
                                                                   static_cast<uint32_t>(m_ptsw->out_two_wind.value("NSEC")))));
    m_fod->out_diode_sec.insert("DRV", static_cast<float>(d_out_two->doDiodeRevVolt(m_indata.input_volt_ac_max)));
    m_fod->out_diode_sec.insert("DPD", static_cast<float>(d_out_two->doDiodePowLoss(m_indata.volt_diode_drop_sec)));
    //
    m_fod->out_diode_thrid.insert("SOP", outPwr(m_indata.curr_out_three,
                                                m_indata.volt_out_three));
    m_fod->out_diode_thrid.insert("SOV", m_indata.volt_out_three);
    m_fod->out_diode_thrid.insert("TR", static_cast<float>(turnRatio(m_ptpe->actual_num_primary,
                                                                     static_cast<uint32_t>(m_ptsw->out_three_wind.value("NSEC")))));
    m_fod->out_diode_thrid.insert("DRV", static_cast<float>(d_out_three->doDiodeRevVolt(m_indata.input_volt_ac_max)));
    m_fod->out_diode_thrid.insert("DPD", static_cast<float>(d_out_three->doDiodePowLoss(m_indata.volt_diode_drop_sec)));
    //
    m_fod->out_diode_four.insert("SOP", outPwr(m_indata.curr_out_four,
                                               m_indata.volt_out_four));
    m_fod->out_diode_four.insert("SOV", m_indata.volt_out_four);
    m_fod->out_diode_four.insert("TR", static_cast<float>(turnRatio(m_ptpe->actual_num_primary,
                                                                    static_cast<uint32_t>(m_ptsw->out_four_wind.value("NSEC")))));
    m_fod->out_diode_four.insert("DRV", static_cast<float>(d_out_four->doDiodeRevVolt(m_indata.input_volt_ac_max)));
    m_fod->out_diode_four.insert("DPD", static_cast<float>(d_out_four->doDiodePowLoss(m_indata.volt_diode_drop_sec)));
    //
    m_fod->out_diode_aux.insert("SOP", outPwr(m_indata.curr_out_aux,
                                              m_indata.volt_out_aux));
    m_fod->out_diode_aux.insert("SOV", m_indata.volt_out_aux);
    m_fod->out_diode_aux.insert("TR", static_cast<float>(turnRatio(m_ptpe->actual_num_primary,
                                                                   static_cast<uint32_t>(m_ptsw->out_aux_wind.value("NAUX")))));
    m_fod->out_diode_aux.insert("DRV", static_cast<float>(d_out_aux->doDiodeRevVolt(m_indata.input_volt_ac_max)));
    m_fod->out_diode_aux.insert("DPD", static_cast<float>(d_out_aux->doDiodePowLoss(m_indata.volt_diode_drop_sec)));

    //Packing output capacitor values
    bool chek_tr = false;
    m_foc->out_cap_first.insert("CVO", c_out_one->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_first.insert("CESRO", c_out_one->ocESRCapOut());
    m_foc->out_cap_first.insert("CCRMS", c_out_one->ocCurrOurRMS(m_ptpe->curr_primary_peak,
                                                                 turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_one_wind.value("NSEC")), chek_tr)
                                                                 ));
    m_foc->out_cap_first.insert("CZFCO", c_out_one->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_first.insert("CRVO", c_out_one->ocOutRippleVolt(m_ptpe->curr_primary_peak,
                                                                   m_foc->out_cap_first.value("CVO"),
                                                                   turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_one_wind.value("NSEC")), chek_tr),
                                                                   m_indata.freq_switch));
    m_foc->out_cap_first.insert("COL", c_out_one->ocCapOutLoss(m_foc->out_cap_first.value("CCRMS")));
    //
    m_foc->out_cap_sec.insert("CVO", c_out_two->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_sec.insert("CESRO", c_out_two->ocESRCapOut());
    m_foc->out_cap_sec.insert("CCRMS", c_out_two->ocCurrOurRMS(m_ptpe->curr_primary_peak,
                                                               turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_two_wind.value("NSEC")), chek_tr)
                                                               ));
    m_foc->out_cap_sec.insert("CZFCO", c_out_two->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_sec.insert("CRVO", c_out_two->ocOutRippleVolt(m_ptpe->curr_primary_peak,
                                                                 m_foc->out_cap_sec.value("CVO"),
                                                                 turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_two_wind.value("NSEC")), chek_tr),
                                                                 m_indata.freq_switch));
    m_foc->out_cap_sec.insert("COL", c_out_two->ocCapOutLoss(m_foc->out_cap_sec.value("CCRMS")));

    m_foc->out_cap_thrid.insert("CVO", c_out_three->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_thrid.insert("CESRO", c_out_three->ocESRCapOut());
    m_foc->out_cap_thrid.insert("CCRMS", c_out_three->ocCurrOurRMS(m_ptpe->curr_primary_peak,
                                                                   turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_three_wind.value("NSEC")), chek_tr)
                                                                   ));
    m_foc->out_cap_thrid.insert("CZFCO", c_out_three->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_thrid.insert("CRVO", c_out_three->ocOutRippleVolt(m_ptpe->curr_primary_peak,
                                                                     m_foc->out_cap_thrid.value("CVO"),
                                                                     turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_three_wind.value("NSEC")), chek_tr),
                                                                     m_indata.freq_switch));
    m_foc->out_cap_thrid.insert("COL", c_out_three->ocCapOutLoss(m_foc->out_cap_thrid.value("CCRMS")));

    m_foc->out_cap_four.insert("CVO", c_out_four->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_four.insert("CESRO", c_out_four->ocESRCapOut());
    m_foc->out_cap_four.insert("CCRMS", c_out_four->ocCurrOurRMS(m_ptpe->curr_primary_peak,
                                                                 turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_four_wind.value("NSEC")), chek_tr)
                                                                 ));
    m_foc->out_cap_four.insert("CZFCO", c_out_four->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_four.insert("CRVO", c_out_four->ocOutRippleVolt(m_ptpe->curr_primary_peak,
                                                                   m_foc->out_cap_four.value("CVO"),
                                                                   turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_four_wind.value("NSEC")), chek_tr),
                                                                   m_indata.freq_switch));
    m_foc->out_cap_four.insert("COL", c_out_four->ocCapOutLoss(m_foc->out_cap_four.value("CCRMS")));

    m_foc->out_cap_aux.insert("CVO", c_out_aux->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_aux.insert("CESRO", c_out_aux->ocESRCapOut());
    m_foc->out_cap_aux.insert("CCRMS", c_out_aux->ocCurrOurRMS(m_ptpe->curr_primary_peak,
                                                               turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_aux_wind.value("NSEC")), chek_tr)
                                                               ));
    m_foc->out_cap_aux.insert("CZFCO", c_out_aux->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_aux.insert("CRVO", c_out_aux->ocOutRippleVolt(m_ptpe->curr_primary_peak,
                                                                 m_foc->out_cap_aux.value("CVO"),
                                                                 turnRatio(m_ptpe->actual_num_primary, static_cast<uint32_t>(m_ptsw->out_aux_wind.value("NSEC")), chek_tr),
                                                                 m_indata.freq_switch));
    m_foc->out_cap_aux.insert("COL", c_out_aux->ocCapOutLoss(m_foc->out_cap_aux.value("CCRMS")));

    qDebug() << "Packing output capacitor and diode values in Thread " << thread()->currentThreadId();

    emit finishedCalcOtputNetwork();
    qDebug() << "Finished calculate output network in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcOutputFilter()
{
    emit startCalcOutputFilter();
    qDebug() << "Start calculate output filter in Thread " << thread()->currentThreadId();

    /**if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    QScopedPointer<OutFilter> out_fl(new OutFilter(m_of->frequency, m_of->load_resistance));

    qDebug() << "Initialise OutFilter object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with OutFilter object in Thread " << thread()->currentThreadId();

    m_of->angular_cut_freq = out_fl->ofAngularCutFreq();
    m_of->capacitor = out_fl->ofCapacitor();
    m_of->inductor = out_fl->ofInductor();
    m_of->q_factor = out_fl->ofQualityFactor();
    m_of->damping = out_fl->ofDampingRatio();
    m_of->cut_freq = out_fl->ofAngularCutFreq();
    m_of->out_ripp_voltage = out_fl->ofOutRipplVolt();
    out_fl->ofPlotArray(m_of->of_freq_array, m_of->of_magnitude_array, m_of->of_phase_array, 10, 1000000, 10);

    emit finishedCalcOutputFilter();
    qDebug() << "Finished calculate output filter in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcPowerStageModel()
{
    emit startCalcPowerStageModel();
    qDebug() << "Start calculate power stage model in Thread " << thread()->currentThreadId();

    /*if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    m_pcssm.reset(new PCSSM(m_ssm));

    qDebug() << "Initialise PCSSM object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with PCSSM object in Thread " << thread()->currentThreadId();

    m_pssm->ps_zero_one = m_pcssm->coZeroOneAngFreq();
    m_pssm->ps_pole_one = m_pcssm->coPoleOneAngFreq();
    m_pssm->ps_dcm_zero_two = m_pcssm->coDCMZeroTwoAngFreq();
    m_pssm->ps_dcm_pole_two = m_pcssm->coDCMPoleTwoAngFreq();
    m_pssm->ps_gain_cmc_mod = m_pcssm->coGainCurrModeContrModulator();

    m_pssm->ps_freq_array.reserve(SET_FREQ_SIZE/100);
    m_pssm->ps_magnitude_array.reserve(SET_FREQ_SIZE/100);
    m_pssm->ps_phase_array.reserve(SET_FREQ_SIZE/100);

    for(int32_t indx =0; indx<SET_FREQ_SIZE; indx +=100)
    {
        m_pssm->ps_freq_array.push_back(indx);
    }

    m_pcssm->coGainControlToOutTransfFunct(m_pssm->ps_freq_array, m_pssm->ps_magnitude_array);
    m_pcssm->coPhaseControlToOutTransfFunct(m_pssm->ps_freq_array, m_pssm->ps_phase_array);

    emit finishedCalcPowerStageModel();
    qDebug() << "Finished calculate power stage model in Thread " << thread()->currentThreadId();
}

void PowSuppSolve::calcOptocouplerFeedback()
{
    emit startCalcOptocouplerFeedback();
    qDebug() << "Start calculate optocoupler feedback in Thread " << thread()->currentThreadId();

    /*if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }*/

    m_fccd.reset(new FCCD(m_fc, m_rs, m_lc));

    qDebug() << "Initialise FCCD object in Thread " << thread()->currentThreadId();

    qDebug() << "Working with FCCD object in Thread " << thread()->currentThreadId();

    m_ofs->of_opto_led_res = m_fccd->coResOptoDiode();
    m_ofs->of_opto_bias_res = m_fccd->coResOptoBias();
    m_ofs->of_up_divide_res = m_fccd->coResUp();

    m_ofs->of_quality = m_fccd->coQuality();
    m_ofs->of_ext_ramp_slope = m_fccd->coExterRampSlope();
    m_ofs->of_ind_on_slope = m_fccd->coIndOnTimeSlope();
    m_ofs->of_freq_cross_sect = m_fccd->coFreqCrossSection();
    m_ofs->of_zero = m_fccd->coFreqZero();
    m_ofs->of_pole = m_fccd->coFreqPole();
    m_ofs->of_cap_opto = m_fccd->coCapPoleOpto();
    m_ofs->of_res_err_amp = m_fccd->coResZero();
    m_ofs->of_cap_err_amp = m_fccd->coCapZero();

    m_ofs->of_freq_array.reserve(SET_FREQ_SIZE/1000);
    m_ofs->of_magnitude_array.reserve(SET_FREQ_SIZE/1000);
    m_ofs->of_phase_array.reserve(SET_FREQ_SIZE/1000);

    for(int32_t indx =0; indx<SET_FREQ_SIZE; indx +=100)
    {
        m_ofs->of_freq_array.push_back(indx);
    }

    m_fccd->coGainOptoFeedbTransfFunc(m_ofs->of_freq_array, m_ofs->of_magnitude_array);
    m_fccd->coPhaseOptoFeedbTransfFunc(m_ofs->of_freq_array, m_ofs->of_phase_array);

    emit finishedCalcOptocouplerFeedback();
    qDebug() << "Finished calculate optocoupler feedback in Thread " << thread()->currentThreadId();
}
