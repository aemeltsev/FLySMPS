#include "powsuppsolve.h"

PowSuppSolve::PowSuppSolve(QObject *parent)
{
    m_indata = new InputValue();
    
    m_db = new DBridge();
    m_bc = new BCap();
    m_pm = new PMosfet();
    m_od = new ODiode();
    m_fod = new FullOutDiode();
    m_oc = new OCap();
    m_foc = new FullOutCap();
    m_of = new FullOutFilter();
    m_pssm = new PowerStageSmallSignalModel();
    m_ofs = new OptocouplerFedbackStage();
    m_ptpe = new PulseTransPrimaryElectr();
    m_ptsw = new PulseTransSecondWired();
}

PowSuppSolve::~PowSuppSolve()
{
    delete m_db;
    delete m_bc;
    delete m_pm;
    delete m_od;
    delete m_fod;
    delete m_oc;
    delete m_foc;
    delete m_of;
    delete m_pssm;
    delete m_ofs;
    delete m_ptpe;
    delete m_ptsw;
}

void PowSuppSolve::setGeneralSecondaryValues(
        int16_t p_volt_out_one,
        int16_t p_curr_out_one,
        int16_t p_volt_out_two,
        int16_t p_curr_out_two,
        int16_t p_volt_out_three,
        int16_t p_curr_out_three,
        int16_t p_volt_out_four,
        int16_t p_curr_out_four,
        int16_t p_volt_out_aux,
        int16_t p_curr_out_aux)
{
    
}

void PowSuppSolve::setGeneralInitValues(
        int16_t p_input_volt_ac_max,
        int16_t p_input_volt_ac_min,
        int16_t p_freq_line,
        int16_t p_freq_switch,
        int16_t p_temp_amb,
        double p_eff,
        double p_power_out_max)
{
    
}

void PowSuppSolve::setGeneralPreDesign(
        int16_t p_refl_volt_max,
        double p_eff_transf,
        int16_t p_voltage_spike,
        double p_volt_diode_drop_sec,
        double p_volt_diode_drop_bridge,
        double pleakage_induct)
{
    
}

void PowSuppSolve::setOutputCap(
        double p_sec_voltage_ripple,
        double p_sec_esr_perc,
        double p_sec_crfq_value)
{
    
}
