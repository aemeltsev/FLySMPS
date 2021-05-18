#include "powsuppsolve.h"

PowSuppSolve::PowSuppSolve(QObject *parent)
    :QObject(parent)
{
    m_db.reset(new DBridge);
    m_bc.reset(new BCap);
    m_pm.reset(new PMosfet);
    m_ptpe.reset(new PulseTransPrimaryElectr());
    m_ptsw.reset(new PulseTransWires());
    //m_od = new ODiode();
    m_fod.reset(new FullOutDiode);
    //m_oc = new OCap();
    m_foc.reset(new FullOutCap);
    m_of.reset(new FullOutFilter);
    m_pssm.reset(new PowerStageSmallSignalModel);
    m_ofs.reset(new OptocouplerFedbackStage);
}

PowSuppSolve::~PowSuppSolve()
{}

void PowSuppSolve::calcInputNetwork()
{
    m_isSolveRunning = true;
    emit startCalcInputNetwork();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    QScopedPointer<BulkCap> b_cap(new BulkCap(m_indata.input_volt_ac_max,
                                              m_indata.input_volt_ac_min,
                                              m_indata.eff,
                                              static_cast<float>(m_indata.power_out_max),
                                              m_indata.freq_line));
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
                                                        m_indata.eff,
                                                        static_cast<float>(m_indata.power_out_max),
                                                        m_indata.freq_line));
    /**< 1. Set capacitor params */
    b_diode->setBcapParam(m_bc->bcapacitor_peak_curr, m_bc->charg_time);
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
    m_isSolveRunning = false;
}

void PowSuppSolve::calcElectricalPrimarySide()
{
    m_isSolveRunning = true;
    emit startCalcElectricalPrimarySide();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    QScopedPointer<FBPTPrimary> t_prim(new FBPTPrimary(m_indata.ripple_fact,
                                                       m_indata.refl_volt_max,
                                                       m_indata.power_out_max,
                                                       m_indata.eff,
                                                       m_indata.freq_switch));
    /**< 1. Set input voltage */
    t_prim->setInputVoltage(m_bc->input_dc_min_voltage, m_bc->input_min_voltage);
    /**< 2. Fill the structure for primary side */
    m_ptpe->max_duty_cycle = t_prim->DutyCycleDCM();
    m_ptpe->inp_power = t_prim->InputPower();
    m_ptpe->primary_induct = t_prim->PriInduct();
    m_ptpe->curr_primary_aver = t_prim->CurrPriAver();
    m_ptpe->curr_primary_peak_peak = t_prim->CurrPriPeakToPeak();
    m_ptpe->curr_primary_peak = t_prim->CurrPriMax();
    m_ptpe->curr_primary_valley = t_prim->CurrPriValley();
    m_ptpe->curr_primary_rms = t_prim->CurrPriRMS();

    emit finishedCalcElectricalPrimarySide();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcArea()
{
    m_isSolveRunning = true;
    emit startCalcArea();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    m_core.reset(new FBPTCore(m_ca, m_ptpe->primary_induct,
                              m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms,
                              m_ptpe->curr_primary_peak_peak, m_indata.power_out_max));
    m_ptpe->core_area_product = m_core->CoreAreaProd();
    m_ptpe->core_geom_coeff = m_core->CoreGeometryCoeff(m_indata.power_out_max);

    emit finishedCalcArea();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcElectroMagProperties()
{
    m_isSolveRunning = true;
    emit startCalcElectroMagProperties();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    QVector<QPair<float, float>> out_vlcr;
    out_vlcr[0] = qMakePair(static_cast<float>(m_indata.volt_out_one),
                            static_cast<float>(m_indata.curr_out_one));
    out_vlcr[1] = qMakePair(static_cast<float>(m_indata.volt_out_two),
                            static_cast<float>(m_indata.curr_out_two));
    out_vlcr[2] = qMakePair(static_cast<float>(m_indata.volt_out_three),
                            static_cast<float>(m_indata.curr_out_three));
    out_vlcr[3] = qMakePair(static_cast<float>(m_indata.volt_out_four),
                            static_cast<float>(m_indata.curr_out_four));
    m_ptpe->curr_dens = m_core->CurrentDens(m_cs);
    m_ptpe->number_primary = m_core->numPrimary(m_cs, m_fns);
    m_ptpe->length_air_gap = m_core->agLength(m_cs, m_ptpe->number_primary);
    m_ptpe->fring_flux_fact = m_core->agFringFluxFact(m_cs, m_ptpe->number_primary, m_fsag, m_md);
    m_ptpe->actual_num_primary = m_core->actNumPrimary(m_cs, m_fsag,
                                                       m_md, m_ptpe->number_primary,
                                                       m_ptpe->primary_induct,
                                                       m_ptpe->curr_primary_peak);
    m_ptpe->actual_flux_dens_peak = m_core->actMagneticFluxPeak(m_cs, m_ptpe->actual_num_primary,
                                                                m_ptpe->curr_primary_peak,
                                                                m_ptpe->fring_flux_fact);
    m_ptpe->actual_max_duty_cycle = m_core->actDutyCycle(out_vlcr, m_bc->input_dc_min_voltage,
                                                         m_indata.freq_switch, m_ptpe->primary_induct);
    m_ptpe->actual_volt_reflected = m_core->actReflVoltage(m_ptpe->actual_max_duty_cycle, m_indata.power_out_max,
                                                           m_ptpe->primary_induct, m_indata.freq_switch);
    emit finishedCalcElectroMagProperties();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcTransformerWired()
{
    m_isSolveRunning = true;
    emit startCalcTransformerWired();
    m_sec.reserve(SET_SECONDARY_WIRED); /**< Reserve for number of secondary side */
    m_wind.reserve(SET_SECONDARY_WIRED+1); /**< Reserve for all windings */

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }
    // Make secondary side objects
    QScopedPointer<FBPTSecondary> sec_one(new FBPTSecondary(m_indata.curr_out_one, m_indata.volt_out_one,
                                                            m_ptpe->actual_volt_reflected, m_indata.power_out_max,
                                                            m_ptpe->actual_num_primary, m_ptpe->actual_max_duty_cycle,
                                                            m_indata.volt_diode_drop_sec));

    QScopedPointer<FBPTSecondary> sec_two(new FBPTSecondary(m_indata.volt_out_two, m_indata.curr_out_two,
                                                            m_ptpe->actual_volt_reflected, m_indata.power_out_max,
                                                            m_ptpe->actual_num_primary, m_ptpe->actual_max_duty_cycle,
                                                            m_indata.volt_diode_drop_sec));

    QScopedPointer<FBPTSecondary> sec_three(new FBPTSecondary(m_indata.volt_out_three, m_indata.curr_out_three,
                                                              m_ptpe->actual_volt_reflected, m_indata.power_out_max,
                                                              m_ptpe->actual_num_primary, m_ptpe->actual_max_duty_cycle,
                                                              m_indata.volt_diode_drop_sec));

    QScopedPointer<FBPTSecondary> sec_four(new FBPTSecondary(m_indata.volt_out_four, m_indata.curr_out_four,
                                                             m_ptpe->actual_volt_reflected, m_indata.power_out_max,
                                                             m_ptpe->actual_num_primary, m_ptpe->actual_max_duty_cycle,
                                                             m_indata.volt_diode_drop_sec));

    QScopedPointer<FBPTSecondary> aux_out(new FBPTSecondary(m_indata.volt_out_aux, m_indata.curr_out_aux,
                                                            m_ptpe->actual_volt_reflected, m_indata.power_out_max,
                                                            m_ptpe->actual_num_primary, m_ptpe->actual_max_duty_cycle,
                                                            m_indata.volt_diode_drop_sec));
    // Packing of the secondary side objects
    m_sec.push_back(sec_one);
    m_sec.push_back(sec_two);
    m_sec.push_back(sec_three);
    m_sec.push_back(sec_four);

    // Make winding objects
    QScopedPointer<FBPTWinding> wind_prim(new FBPTWinding(m_ptpe->actual_num_primary, m_indata.freq_switch,
                                                          m_ptpe->curr_primary_rms, m_psw.m_mcd,
                                                          m_psw.m_fcu, m_psw.m_ins[0]));

    QScopedPointer<FBPTWinding> wind_sec_one(new FBPTWinding(m_ptpe->actual_num_primary, m_indata.freq_switch,
                                                             m_ptpe->curr_primary_rms, m_psw.m_mcd,
                                                             m_psw.m_fcu, m_psw.m_ins[1]));

    QScopedPointer<FBPTWinding> wind_sec_two(new FBPTWinding(m_ptpe->actual_num_primary, m_indata.freq_switch,
                                                             m_ptpe->curr_primary_rms, m_psw.m_mcd,
                                                             m_psw.m_fcu, m_psw.m_ins[2]));

    QScopedPointer<FBPTWinding> wind_sec_three(new FBPTWinding(m_ptpe->actual_num_primary, m_indata.freq_switch,
                                                               m_ptpe->curr_primary_rms, m_psw.m_mcd,
                                                               m_psw.m_fcu, m_psw.m_ins[3]));

    QScopedPointer<FBPTWinding> wind_sec_four(new FBPTWinding(m_ptpe->actual_num_primary, m_indata.freq_switch,
                                                              m_ptpe->curr_primary_rms, m_psw.m_mcd,
                                                              m_psw.m_fcu, m_psw.m_ins[4]));

    QScopedPointer<FBPTWinding> wind_aux(new FBPTWinding(m_ptpe->actual_num_primary, m_indata.freq_switch,
                                                         m_ptpe->curr_primary_rms, m_psw.m_mcd,
                                                         m_psw.m_fcu, m_psw.m_ins[5]));
    // Packing of the winding objects in to sequence container
    m_wind.push_back(wind_prim);
    m_wind.push_back(wind_sec_one);
    m_wind.push_back(wind_sec_two);
    m_wind.push_back(wind_sec_three);
    m_wind.push_back(wind_sec_four);
    m_wind.push_back(wind_aux);

    // Packing winding properties of the primary side
    m_ptsw->primary_wind.insert("AP", m_wind[0]->wCoperWireCrossSectArea(m_cs, m_md, m_psw.m_af[0]));
    m_ptsw->primary_wind.insert("AWGP", m_wind[0]->wMaxWireSizeAWG(m_ptsw->primary_wind.value("AP")));

    m_wind[0]->setWireDiam(m_ptsw->primary_wind.value("AWGP"), m_ptpe->actual_num_primary);

    m_ptsw->primary_wind.insert("DP", m_wind[0]->wCoperWireDiam());
    m_ptsw->primary_wind.insert("ECA", m_wind[0]->wCoperWireCrossSectAreaPost());
    m_ptsw->primary_wind.insert("JP", m_wind[0]->wCurrentDenst());
    m_ptsw->primary_wind.insert("OD", 1);
    m_ptsw->primary_wind.insert("NTL", m_wind[0]->wNumTurnToLay(m_md));
    m_ptsw->primary_wind.insert("LN", m_wind[0]->wNumLay(m_md));

    // Packing winding properties of the 1st secondary side
    m_sec[0]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_one_wind.insert("JSP", m_sec[0]->outCurrPeakSecond());
    m_ptsw->out_one_wind.insert("JSRMS", m_sec[0]->outCurrRMSSecond());
    m_ptsw->out_one_wind.insert("NSEC", m_sec[0]->outNumSecond());
    m_ptsw->out_one_wind.insert("ANS", m_wind[1]->wCoperWireCrossSectArea(m_cs, m_md, m_psw.m_af[1]));
    m_ptsw->out_one_wind.insert("AWGNS", m_wind[1]->wMaxWireSizeAWG(m_ptsw->out_one_wind.value("ANS")));

    m_wind[1]->setWireDiam(m_ptsw->out_one_wind.value("AWGNS"), m_ptpe->actual_num_primary);
    m_ptsw->out_one_wind.insert("DS", m_wind[1]->wCoperWireDiam());
    m_ptsw->out_one_wind.insert("ECA", m_wind[1]->wCoperWireCrossSectAreaPost());
    m_ptsw->out_one_wind.insert("JS", m_wind[1]->wCurrentDenst());
    m_ptsw->out_one_wind.insert("OD", 1);
    m_ptsw->out_one_wind.insert("NTL", m_wind[1]->wNumTurnToLay(m_md));
    m_ptsw->out_one_wind.insert("LN", m_wind[1]->wNumLay(m_md));

    // Packing winding properties of the 2nd secondary side
    m_sec[1]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_two_wind.insert("JSP", m_sec[1]->outCurrPeakSecond());
    m_ptsw->out_two_wind.insert("JSRMS", m_sec[1]->outCurrRMSSecond());
    m_ptsw->out_two_wind.insert("NSEC", m_sec[1]->outNumSecond());
    m_ptsw->out_two_wind.insert("ANS", m_wind[2]->wCoperWireCrossSectArea(m_cs, m_md, m_psw.m_af[2]));
    m_ptsw->out_two_wind.insert("AWGNS", m_wind[2]->wMaxWireSizeAWG(m_ptsw->out_two_wind.value("ANS")));

    m_wind[2]->setWireDiam(m_ptsw->out_two_wind.value("AWGNS"), m_ptpe->actual_num_primary);

    m_ptsw->out_two_wind.insert("DS", m_wind[2]->wCoperWireDiam());
    m_ptsw->out_two_wind.insert("ECA", m_wind[2]->wCoperWireCrossSectAreaPost());
    m_ptsw->out_two_wind.insert("JS", m_wind[2]->wCurrentDenst());
    m_ptsw->out_two_wind.insert("OD", 1);
    m_ptsw->out_two_wind.insert("NTL", m_wind[2]->wNumTurnToLay(m_md));
    m_ptsw->out_two_wind.insert("LN", m_wind[2]->wNumLay(m_md));

    // Packing winding properties of the 3th secondary side
    m_sec[2]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_three_wind.insert("JSP", m_sec[2]->outCurrPeakSecond());
    m_ptsw->out_three_wind.insert("JSRMS", m_sec[2]->outCurrRMSSecond());
    m_ptsw->out_three_wind.insert("NSEC", m_sec[2]->outNumSecond());
    m_ptsw->out_three_wind.insert("ANS", m_wind[3]->wCoperWireCrossSectArea(m_cs, m_md, m_psw.m_af[3]));
    m_ptsw->out_three_wind.insert("AWGNS", m_wind[3]->wMaxWireSizeAWG(m_ptsw->out_three_wind.value("ANS")));

    m_wind[3]->setWireDiam(m_ptsw->out_three_wind.value("AWGNS"), m_ptpe->actual_num_primary);

    m_ptsw->out_three_wind.insert("DS", m_wind[3]->wCoperWireDiam());
    m_ptsw->out_three_wind.insert("ECA", m_wind[3]->wCoperWireCrossSectAreaPost());
    m_ptsw->out_three_wind.insert("JS", m_wind[3]->wCurrentDenst());
    m_ptsw->out_three_wind.insert("OD", 1);
    m_ptsw->out_three_wind.insert("NTL", m_wind[3]->wNumTurnToLay(m_md));
    m_ptsw->out_three_wind.insert("LN", m_wind[3]->wNumLay(m_md));

    // Packing winding properties of the 4th secondary side
    m_sec[3]->setCurrentParam(m_ptpe->curr_primary_peak, m_ptpe->curr_primary_rms);

    m_ptsw->out_four_wind.insert("JSP", m_sec[3]->outCurrPeakSecond());
    m_ptsw->out_four_wind.insert("JSRMS", m_sec[3]->outCurrRMSSecond());
    m_ptsw->out_four_wind.insert("NSEC", m_sec[3]->outNumSecond());
    m_ptsw->out_four_wind.insert("ANS", m_wind[4]->wCoperWireCrossSectArea(m_cs, m_md, m_psw.m_af[4]));
    m_ptsw->out_four_wind.insert("AWGNS", m_wind[4]->wMaxWireSizeAWG(m_ptsw->out_four_wind.value("ANS")));

    m_wind[4]->setWireDiam(m_ptsw->out_four_wind.value("AWGNS"), m_ptpe->actual_num_primary);

    m_ptsw->out_four_wind.insert("DS", m_wind[4]->wCoperWireDiam());
    m_ptsw->out_four_wind.insert("ECA", m_wind[4]->wCoperWireCrossSectAreaPost());
    m_ptsw->out_four_wind.insert("JS", m_wind[4]->wCurrentDenst());
    m_ptsw->out_four_wind.insert("OD", 1);
    m_ptsw->out_four_wind.insert("NTL", m_wind[4]->wNumTurnToLay(m_md));
    m_ptsw->out_four_wind.insert("LN", m_wind[4]->wNumLay(m_md));

    // Packing winding properties of the auxilary side
    m_ptsw->out_aux_wind.insert("NAUX", aux_out->outNumSecond());
    m_ptsw->out_aux_wind.insert("ANAUX", m_wind[5]->wCoperWireCrossSectArea(m_cs, m_md, m_psw.m_af[5]));
    m_ptsw->out_aux_wind.insert("AWGAUX", m_wind[5]->wMaxWireSizeAWG(m_ptsw->out_aux_wind.value("ANS")));

    m_wind[5]->setWireDiam(m_ptsw->out_aux_wind.value("AWGAUX"), m_ptpe->actual_num_primary);

    m_ptsw->out_aux_wind.insert("DAUX", m_wind[5]->wCoperWireDiam());
    m_ptsw->out_aux_wind.insert("ECA", m_wind[5]->wCoperWireCrossSectAreaPost());
    m_ptsw->out_aux_wind.insert("OD", 1);
    m_ptsw->out_aux_wind.insert("NTL", m_wind[5]->wNumTurnToLay(m_md));

    emit finishedCalcTransformerWired();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcSwitchNetwork()
{
    m_isSolveRunning = true;
    emit startCalcSwitchNetwork();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    int16_t vmaxrms = static_cast<int16_t>(m_indata.input_volt_ac_max * M_SQRT2);
    int16_t vminrms = static_cast<int16_t>(m_indata.input_volt_ac_min * M_SQRT2);

    QScopedPointer<SwMosfet> sw_mos(new SwMosfet(vmaxrms, vminrms,
                                                 m_ptpe->actual_volt_reflected, m_indata.voltage_spike,
                                                 m_indata.eff, m_indata.power_out_max,
                                                 m_indata.freq_switch, m_ptpe->actual_max_duty_cycle,
                                                 m_indata.leakage_induct));
    m_pm->mosfet_voltage_nom = sw_mos->swMosfetVoltageNom();
    m_pm->mosfet_voltage_max = sw_mos->swMosfetVoltageMax();
    sw_mos->setCurrValues(m_ptpe->curr_primary_rms, m_ptpe->curr_primary_peak);
    m_pm->mosfet_ds_curr = sw_mos->swMosfetCurrent();
    m_pm->mosfet_on_time = sw_mos->swMosfetOnTime(m_ptpe->primary_induct, (m_indata.input_volt_ac_max/M_SQRT2));
    m_pm->mosfet_off_time = sw_mos->swMosfetOffTime(m_pm->mosfet_on_time);
    m_pm->mosfet_sw_tot = sw_mos->swMosfetTotPeriod();
    m_pm->mosfet_rise_time = sw_mos->swMosfetRiseTime(m_mospr);
    m_pm->mosfet_conduct_loss = sw_mos->swMosfetConductLoss(m_mospr);
    m_pm->mosfet_drive_loss = sw_mos->swMosfetDriveLoss(m_mospr);
    m_pm->mosfet_switch_loss = sw_mos->swMosfetSwitchLoss(m_mospr);
    m_pm->mosfet_capacit_loss = sw_mos->swMosfetCapacitLoss(m_mospr);
    m_pm->mosfet_total_loss = sw_mos->swMosfetTotalLoss(m_mospr);

    m_pm->snubber_voltage_max = sw_mos->clVoltageMax();
    m_pm->snubber_pwr_diss = sw_mos->clPowerDiss(m_ccsp);
    m_pm->snubber_res_value = sw_mos->clResValue(m_ccsp);
    m_pm->snubber_cap_value = sw_mos->clCapValue(m_ccsp);

    m_pm->curr_sense_res = sw_mos->csCurrRes(m_ccsp);
    m_pm->curr_sense_res_loss = sw_mos->csCurrResLoss(m_ccsp);

    emit finishedCalcSwitchNetwork();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcOtputNetwork()
{
    m_isSolveRunning = true;
    emit startCalcOtputNetwork();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    //Construct output diode objects
    QScopedPointer<DiodeOut> d_out_one(new DiodeOut((m_indata.curr_out_one * m_indata.volt_out_one), m_indata.volt_out_one,
                                                    (m_ptpe->actual_num_primary * m_ptsw->out_one_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_two(new DiodeOut((m_indata.curr_out_two*m_indata.volt_out_two), m_indata.volt_out_two,
                                                    (m_ptpe->actual_num_primary * m_ptsw->out_two_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_three(new DiodeOut((m_indata.curr_out_three*m_indata.volt_out_three), m_indata.volt_out_three,
                                                      (m_ptpe->actual_num_primary * m_ptsw->out_three_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_four(new DiodeOut((m_indata.curr_out_four*m_indata.volt_out_four), m_indata.volt_out_four,
                                                     (m_ptpe->actual_num_primary * m_ptsw->out_four_wind.value("NSEC"))));

    QScopedPointer<DiodeOut> d_out_aux(new DiodeOut((m_indata.curr_out_aux*m_indata.volt_out_aux), m_indata.volt_out_aux,
                                                    (m_ptpe->actual_num_primary * m_ptsw->out_aux_wind.value("NAUX"))));
    //Construct output capacitor objects
    QScopedPointer<CapOut> c_out_one(new CapOut(m_cop));
    QScopedPointer<CapOut> c_out_two(new CapOut(m_cop));
    QScopedPointer<CapOut> c_out_three(new CapOut(m_cop));
    QScopedPointer<CapOut> c_out_four(new CapOut(m_cop));
    QScopedPointer<CapOut> c_out_aux(new CapOut(m_cop));

    //Packing output diode values
    m_fod->out_diode_first.insert("SOP", (m_indata.curr_out_one * m_indata.volt_out_one));
    m_fod->out_diode_first.insert("SOV", m_indata.volt_out_one);
    m_fod->out_diode_first.insert("TR", (m_ptpe->actual_num_primary * m_ptsw->out_one_wind.value("NSEC")));
    m_fod->out_diode_first.insert("DRV", d_out_one->doDiodeRevVolt(m_indata.input_volt_ac_max));
    m_fod->out_diode_first.insert("DPD", d_out_aux->doDiodePowLoss(m_indata.volt_diode_drop_sec));

    m_fod->out_diode_sec.insert("SOP", (m_indata.curr_out_two*m_indata.volt_out_two));
    m_fod->out_diode_sec.insert("SOV", m_indata.volt_out_two);
    m_fod->out_diode_sec.insert("TR", (m_ptpe->actual_num_primary * m_ptsw->out_two_wind.value("NSEC")));
    m_fod->out_diode_sec.insert("DRV", d_out_two->doDiodeRevVolt(m_indata.input_volt_ac_max));
    m_fod->out_diode_sec.insert("DPD", d_out_two->doDiodePowLoss(m_indata.volt_diode_drop_sec));

    m_fod->out_diode_thrid.insert("SOP", (m_indata.curr_out_three*m_indata.volt_out_three));
    m_fod->out_diode_thrid.insert("SOV", m_indata.volt_out_three);
    m_fod->out_diode_thrid.insert("TR", (m_ptpe->actual_num_primary * m_ptsw->out_three_wind.value("NSEC")));
    m_fod->out_diode_thrid.insert("DRV", d_out_three->doDiodeRevVolt(m_indata.input_volt_ac_max));
    m_fod->out_diode_thrid.insert("DPD", d_out_three->doDiodePowLoss(m_indata.volt_diode_drop_sec));

    m_fod->out_diode_four.insert("SOP", (m_indata.curr_out_four*m_indata.volt_out_four));
    m_fod->out_diode_four.insert("SOV", m_indata.volt_out_four);
    m_fod->out_diode_four.insert("TR", (m_ptpe->actual_num_primary * m_ptsw->out_four_wind.value("NSEC")));
    m_fod->out_diode_four.insert("DRV", d_out_four->doDiodeRevVolt(m_indata.input_volt_ac_max));
    m_fod->out_diode_four.insert("DPD", d_out_four->doDiodePowLoss(m_indata.volt_diode_drop_sec));

    m_fod->out_diode_aux.insert("SOP", (m_indata.curr_out_aux*m_indata.volt_out_aux));
    m_fod->out_diode_aux.insert("SOV", m_indata.volt_out_aux);
    m_fod->out_diode_aux.insert("TR", (m_ptpe->actual_num_primary * m_ptsw->out_aux_wind.value("NAUX")));
    m_fod->out_diode_aux.insert("DRV", d_out_aux->doDiodeRevVolt(m_indata.input_volt_ac_max));
    m_fod->out_diode_aux.insert("DPD", d_out_aux->doDiodePowLoss(m_indata.volt_diode_drop_sec));

    //Packing output capacitor values
    m_foc->out_cap_first.insert("CVO", c_out_one->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_first.insert("CESRO", c_out_one->ocESRCapOut());
    m_foc->out_cap_first.insert("CCRMS", c_out_one->ocCurrOurRMS(m_ptpe->actual_max_duty_cycle));
    m_foc->out_cap_first.insert("CZFCO", c_out_one->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_first.insert("CRVO", c_out_one->ocOutRippleVolt(m_indata.curr_out_one, 1));
    m_foc->out_cap_first.insert("COL", c_out_one->ocCapOutLoss(m_ptpe->actual_max_duty_cycle));

    m_foc->out_cap_sec.insert("CVO", c_out_two->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_sec.insert("CESRO", c_out_two->ocESRCapOut());
    m_foc->out_cap_sec.insert("CCRMS", c_out_two->ocCurrOurRMS(m_ptpe->actual_max_duty_cycle));
    m_foc->out_cap_sec.insert("CZFCO", c_out_two->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_sec.insert("CRVO", c_out_two->ocOutRippleVolt(m_indata.curr_out_two, 1));
    m_foc->out_cap_sec.insert("COL", c_out_two->ocCapOutLoss(m_ptpe->actual_max_duty_cycle));

    m_foc->out_cap_thrid.insert("CVO", c_out_three->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_thrid.insert("CESRO", c_out_three->ocESRCapOut());
    m_foc->out_cap_thrid.insert("CCRMS", c_out_three->ocCurrOurRMS(m_ptpe->actual_max_duty_cycle));
    m_foc->out_cap_thrid.insert("CZFCO", c_out_three->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_thrid.insert("CRVO", c_out_three->ocOutRippleVolt(m_indata.curr_out_three, 1));
    m_foc->out_cap_thrid.insert("COL", c_out_three->ocCapOutLoss(m_ptpe->actual_max_duty_cycle));

    m_foc->out_cap_four.insert("CVO", c_out_four->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_four.insert("CESRO", c_out_four->ocESRCapOut());
    m_foc->out_cap_four.insert("CCRMS", c_out_four->ocCurrOurRMS(m_ptpe->actual_max_duty_cycle));
    m_foc->out_cap_four.insert("CZFCO", c_out_four->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_four.insert("CRVO", c_out_four->ocOutRippleVolt(m_indata.curr_out_four, 1));
    m_foc->out_cap_four.insert("COL", c_out_four->ocCapOutLoss(m_ptpe->actual_max_duty_cycle));

    m_foc->out_cap_aux.insert("CVO", c_out_aux->ocCapOutValue(m_indata.freq_switch));
    m_foc->out_cap_aux.insert("CESRO", c_out_aux->ocESRCapOut());
    m_foc->out_cap_aux.insert("CCRMS", c_out_aux->ocCurrOurRMS(m_ptpe->actual_max_duty_cycle));
    m_foc->out_cap_aux.insert("CZFCO", c_out_aux->ocZeroFreqCapOut(m_indata.freq_switch));
    m_foc->out_cap_aux.insert("CRVO", c_out_aux->ocOutRippleVolt(m_indata.curr_out_aux, 1));
    m_foc->out_cap_aux.insert("COL", c_out_aux->ocCapOutLoss(m_ptpe->actual_max_duty_cycle));

    emit finishedCalcOtputNetwork();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcOutputFilter()
{
    m_isSolveRunning = true;
    emit startCalcOutputFilter();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    QScopedPointer<OutFilter> out_fl(new OutFilter(m_of->frequency, m_of->load_resistance));

    m_of->angular_cut_freq = out_fl->ofAngularCutFreq();
    m_of->capacitor = out_fl->ofCapacitor();
    m_of->inductor = out_fl->ofInductor();
    m_of->q_factor = out_fl->ofQualityFactor();
    m_of->damping = out_fl->ofDampingRatio();
    m_of->cut_freq = out_fl->ofAngularCutFreq();
    m_of->out_ripp_voltage = out_fl->ofOutRipplVolt();
    out_fl->ofPlotArray(m_of->of_freq_array, m_of->of_magnitude_array, m_of->of_phase_array, 10, 1000000, 10);

    emit finishedCalcOutputFilter();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcPowerStageModel()
{
    m_isSolveRunning = true;
    emit startCalcPowerStageModel();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    m_pcssm.reset(new PCSSM(m_ssm));

    m_pssm->ps_zero_one = m_pcssm->coZeroOneAngFreq();
    m_pssm->ps_pole_one = m_pcssm->coPoleOneAngFreq();
    m_pssm->ps_dcm_zero_two = m_pcssm->coDCMZeroTwoAngFreq();
    m_pssm->ps_dcm_pole_two = m_pcssm->coDCMPoleTwoAngFreq();
    m_pssm->ps_gain_cmc_mod = m_pcssm->coGainCurrModeContrModulator();
    m_pssm;
    m_pssm;
    m_pssm;

    emit finishedCalcPowerStageModel();
    m_isSolveRunning = false;
}

void PowSuppSolve::calcOptocouplerFeedback()
{
    m_isSolveRunning = true;
    emit startCalcOptocouplerFeedback();

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }

    m_fccd.reset(new FCCD)

    emit finishedCalcOptocouplerFeedback();
    m_isSolveRunning = false;
}
