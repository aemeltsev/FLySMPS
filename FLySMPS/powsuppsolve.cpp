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

    if(!m_isSolveRunning){
        emit calcCanceled();
        return;
    }


}



