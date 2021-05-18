#include "controlout.h"

/********************COM*************************/
/**
 * @brief coZeroOneAngFreq - $\omega_{zc}$ - esr zero, lhp
 * @return esr zero value, angular frequency
 */
inline double PCSSM::coZeroOneAngFreq() const
{
    return 1/(m_ssmvar.output_cap * m_ssmvar.output_cap_esr);
}

/**
 * @brief coPoleOneAngFreq - $\omega_{rc}$ the dominant pole
 * @return
 */
inline double PCSSM::coPoleOneAngFreq() const
{
    return 2/(static_cast<double>(m_ssmvar.output_full_load_res) * m_ssmvar.output_cap);
}
/**********************DCM************************/
/**
 * @brief coZeroTwoAngFreq - $\omega_{zrhp}$ - rhp zero
 * @return
 */
inline double PCSSM::coDCMZeroTwoAngFreq() const
{
    double voltrat = m_ssmvar.output_voltage/m_ssmvar.input_voltage;
    double tmp = qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res);

    return tmp/(m_ssmvar.primary_ind * voltrat*(voltrat+1));
}

/**
 * @brief coPoleTwoAngFreq - $\omega_{p2}$ - pole
 * @return
 */
inline double PCSSM::coDCMPoleTwoAngFreq() const
{
    double voltrat = m_ssmvar.output_voltage/m_ssmvar.input_voltage;

    return (qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res))
            /(m_ssmvar.primary_ind * qPow((voltrat+1),2));
}

/**
 * @brief coDCMCriticValue - $K_{vd}$ -
 * @param fsw
 * @return
 */
inline double PCSSM::coDCMCriticValue() const
{
    double ktmp = (2. * m_ssmvar.primary_ind * static_cast<double>(m_ssmvar.freq_switch))
            /static_cast<double>(m_ssmvar.output_full_load_res);

    return static_cast<double>(m_ssmvar.input_voltage)
            /(static_cast<double>(m_ssmvar.turn_ratio) * qSqrt(ktmp));
}
/***********************F_m************************/
/**
 * @brief coCurrDetectSlopeVolt - $S_{n}$ - the voltage slope when the primary-side current is detected on RS_s
 *        The inductor rising slope.
 * @return
 */
inline double PCSSM::coCurrDetectSlopeVolt() const
{
    return (m_ssmvar.input_voltage * m_ssmvar.res_sense)
            / m_ssmvar.primary_ind;
}

/**
 * @brief coTimeConst - $\tau_{L}$ - switching period
 * @return
 */
inline double PCSSM::coTimeConst() const
{
    return (2 * m_ssmvar.primary_ind * static_cast<double>(m_ssmvar.freq_switch))
            /(qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res));
}

/**
 * @brief coGainCurrModeContrModulator - $F_{m}$ - the PWM modulator gain
 * @return
 */
inline double PCSSM::coGainCurrModeContrModulator() const
{
    return 1/((coCurrDetectSlopeVolt()+sawvolt)*coTimeConst());
}
/**********************OUT*************************/
/**
 * @brief coDutyToOutTrasfFunct - $G_{vd}(s)$ -
 * @param s
 * @return
 */
inline double PCSSM::coDutyToOutTrasfFunct(double s)
{
    double result = 0.0;
    double num1 = 0.0;
    double num2 = 0.0;
    double dnm1 = 0.0;
    double dnm2 = 0.0;
    if(m_mode == DCM_MODE)
    {
        num1 = 1+(s/coZeroOneAngFreq());
        num2 = 1-(s/coDCMZeroTwoAngFreq());
        dnm1 = 1+(s/coPoleOneAngFreq());
        dnm2 = 1+(s/coDCMPoleTwoAngFreq());
        result = coDCMCriticValue()*((num1*num2)/(dnm1*dnm2));
    }
    else if(m_mode == CCM_MODE)
    {
        num1 = 1-(s/coCCMZeroTwoAngFreq());
        num2 = 1+(s/coZeroOneAngFreq());
        dnm1 = s/(coCCMQualityFact()*coCCMPoleTwoAngFreq());
        dnm2 = std::pow((s/coCCMPoleTwoAngFreq()), 2);
        result = coCCMVoltGainCoeff()*((num1*num2)/(1+dnm1+dnm2));
    }
    else
    {
        result = -1; //it's very bad way, i will rewrite this after
    }
    return result;
}

/**
 * @brief coControlToOutTransfFunct - $G_{vc}(s)$ - The control-to-output transfer function
 * @param s
 * @return
 */
inline double PCSSM::coControlToOutTransfFunct(double s)
{
    double result = 0.0;
    double dnm = 0.0;
    if(m_mode == DCM_MODE)
    {
        result = coDutyToOutTrasfFunct(s)*coGainCurrModeContrModulator();
    }
    else if(m_mode == CCM_MODE)
    {
        dnm = coGainCurrModeContrModulator()* m_ssmvar.res_sense *coCCMDutyToInductCurrTrasfFunct(s);
        result = (coGainCurrModeContrModulator() * coDutyToOutTrasfFunct(s))/(1+dnm);
    }
    else
    {
        result = -1;
    }
    return result;
}
/**********************CCM****************************/
/**
 * @brief coCCMZeroTwoAngFreq - $\omega_{zrhp}$
 * @return
 */
inline double PCSSM::coCCMZeroTwoAngFreq() const
{
    double tmp = qPow(m_ssmvar.turn_ratio, 2) * qPow((1-m_ssmvar.actual_duty), 2)
            * static_cast<double>(m_ssmvar.output_full_load_res);

    return tmp/(m_ssmvar.actual_duty * m_ssmvar.primary_ind);
}

/**
 * @brief coCCMPoleTwoAngFreq - $\omega_{o}$
 * @return
 */
inline double PCSSM::coCCMPoleTwoAngFreq() const
{
    double tmp = static_cast<double>(m_ssmvar.turn_ratio)
            /(qSqrt(m_ssmvar.output_cap * m_ssmvar.primary_ind));

    double num = qPow((1-m_ssmvar.actual_duty), 2);

    double ld = qSqrt((num*static_cast<double>(m_ssmvar.output_full_load_res))
                      /(static_cast<double>(m_ssmvar.output_full_load_res)+m_ssmvar.output_cap_esr));
    return tmp*ld;
}

/**
 * @brief coDCMVoltGainCoeff - $K_{vd}$
 * @return
 */
inline double PCSSM::coCCMVoltGainCoeff() const
{
    return static_cast<double>(m_ssmvar.input_voltage)
            /(static_cast<double>(m_ssmvar.turn_ratio) * (qPow((1-m_ssmvar.actual_duty),2)));
}

/**
 * @brief coDCMCurrGainCoeff - $K_{id}$
 * @return
 */
inline double PCSSM::coCCMCurrGainCoeff() const
{
    double tmp = 1+(2*m_ssmvar.actual_duty/(1-m_ssmvar.actual_duty));

    double dnm = qPow((1-m_ssmvar.actual_duty), 2);

    double mult = m_ssmvar.input_voltage
            /(static_cast<double>(m_ssmvar.turn_ratio) * dnm * static_cast<double>(m_ssmvar.output_full_load_res));

    return tmp*mult;
}

/**
 * @brief coDCMQualityFact - $Q$
 * @return
 */
inline double PCSSM::coCCMQualityFact() const
{
    double dnm1 = m_ssmvar.primary_ind
            /(qPow(static_cast<double>(m_ssmvar.turn_ratio), 2)
            * qPow((1-m_ssmvar.actual_duty), 2) * static_cast<double>(m_ssmvar.output_full_load_res));

    double dnm2 = m_ssmvar.output_cap_esr * static_cast<double>(m_ssmvar.output_full_load_res);

    return 1/(coCCMPoleTwoAngFreq()*(dnm1+dnm2));
}

/**
 * @brief coDCMDutyToInductCurrTrasfFunct - $G_{id}(s)$
 * @param s
 * @return
 */
inline double PCSSM::coCCMDutyToInductCurrTrasfFunct(double s)
{
    double num = 1+(s/coPoleOneAngFreq());

    double dnm = 1+(s/(coCCMQualityFact()*coCCMPoleTwoAngFreq()))+qPow((s/coCCMPoleTwoAngFreq()), 2);

    return coCCMCurrGainCoeff() * (num/dnm);
}

/**************************FCCD*************************/
/**
 * @brief coResOptoDiode -
 * @return
 */
inline int16_t FCCD::coResOptoDiode() const
{
    double num = volt_to_cont-S_OPTO_FORVARD_DROP-S_TL431_VREF;
    double dnm = S_INT_BIAS_CONTR-S_OPTO_CE_SAT+(S_TL431_CURR_CATH*opto_ctr*res_pull_up);
    return static_cast<int16_t>((num/dnm)*0.15);//15% marg
}

/**
 * @brief coResOptoBias -
 * @return
 */
inline int16_t FCCD::coResOptoBias() const
{
    double num = S_OPTO_FORVARD_DROP+(coResOptoDiode()*((S_INT_BIAS_CONTR-3)/(res_pull_up*opto_ctr)));
    return static_cast<int16_t>(num/S_TL431_CURR_CATH);
}

/**
 * @brief coResUp
 * @return
 */
inline int16_t FCCD::coResUp() const
{
    return res_down * static_cast<int16_t>((volt_to_cont-S_TL431_VREF)/S_TL431_VREF);
}

/**
 * @brief FCCD::coVoltageDivideGain
 * @return
 */
inline double FCCD::coVoltageDivideGain() const
{
    return res_down/(coResUp()+res_down);
}

/**
 * @brief FCCD::coVoltageOptoGain
 * @return
 */
inline double FCCD::coVoltageOptoGain() const
{
    return (res_pull_up/coResOptoDiode())*opto_ctr;
}

/**
 * @brief FCCD::coIndOnTimeSlope - S_{n}
 * @return
 */
inline double FCCD::coIndOnTimeSlope() const
{
    return (volt_inp*res_cur_sense)/induct_prim;
}

/**
 * @brief FCCD::coCompRamp - M_{c}
 * @return
 */
inline double FCCD::coCompRamp() const
{
    double coeff = number_prim_turns/number_sec_turns;
    return (coeff*volt_to_cont)/volt_inp;
}

/**
 * @brief FCCD::coExterRampSlope - S_{e}
 * @return
 */
inline double FCCD::coExterRampSlope() const
{
    return (coCompRamp()-1)*coIndOnTimeSlope();
}

/**
 * @brief FCCD::coQuality - Q_{p}
 * @return
 */
inline double FCCD::coQuality() const
{
    double inv_duty = 1-duty_cycle;
    return 1/(M_PI*(coCompRamp()*inv_duty-0.5));
}

/**
 * @brief coFreqCrossSection - f_{cross}
 * @return
 */
inline double FCCD::coFreqCrossSection() const
{
    double ratio_factor = qPow((number_prim_turns/number_sec_turns), 2);
    double coeff = 1/5;
    double num = (qPow(volt_to_cont, 2)/power_out_tot)*qPow(duty_cycle, 2);
    double dnm = 2*M_PI*induct_prim;
    return (num/dnm)*ratio_factor*coeff;
}

/**
 * @brief coKFactor
 * @return
 */
inline double FCCD::coKFactor() const
{
    return qTan((coBoost()/2)+45);
}

/**
 * @brief coFreqPole
 * @return
 */
inline double FCCD::coFreqPole() const
{
    return coKFactor()*coFreqCrossSection();
}

/**
 * @brief coFreqZero
 * @return
 */
inline double FCCD::coFreqZero() const
{
    return coFreqCrossSection()/coKFactor();
}

/**
 * @brief coCapPoleOpto
 * @return
 */
inline double FCCD::coCapPoleOpto() const
{
    return opto_ctr/(2*M_PI*coFreqPole()*coResOptoDiode());
}

/**
 * @brief coCapOpto
 * @return
 */
inline double FCCD::coCapOpto() const
{
    return coCapPoleOpto()-opto_inner_cap;
}

/**
 * @brief coGainErrorAmp
 * @param ControlToOut
 * @return
 */
inline double FCCD::coGainErrorAmp(double control_to_out) const
{
    return qAbs(20*std::log10(coVoltageDivideGain())+20*std::log10(control_to_out)+20*std::log10(coVoltageOptoGain()));
}

/**
 * @brief coResErrorAmp
 * @param control_to_out
 * @return
 */
inline double FCCD::coResErrorAmp(double control_to_out) const
{
    return qPow(10,(coGainErrorAmp(control_to_out)/20))*((coResUp()*res_down)/(coResUp()+res_down));
}

/**
 * @brief coCapErroAmp -
 * @param frq_ea_zero
 * @param res_ea
 * @return
 */
inline double FCCD::coCapErroAmp(int16_t frq_ea_zero, int16_t res_ea) const
{
    return 1/(2*M_PI*frq_ea_zero*(coResUp()+res_ea));
}

/**
 * @brief coOptoTransfGain - $K_{c}$
 * @return
 */
inline double FCCD::coOptoTransfGain() const
{
    double kd = res_down/static_cast<double>((coResUp()+res_down));
    return (opto_ctr*kd*res_pull_up)/coResOptoDiode();
}

/**
 * @brief coTransfZero - $\omega_{z}$
 * @return
 */
inline double FCCD::coTransfZero(double control_to_out, int16_t frq_ea_zero, int16_t res_ea) const
{
    return 1/((coResErrorAmp(control_to_out)+coResUp())*coCapErroAmp(frq_ea_zero, res_ea));
}

/**
 * @brief coTransfPoleOne - $\omega_{p1}$
 * @return
 */
inline double FCCD::coTransfPoleOne() const
{
    return 1/(res_pull_up*coCapOpto());
}

/**
 * @brief coOptoFeedbTransfFunc -
 * @param freq
 * @param mode
 * @return
 */
inline double FCCD::coOptoFeedbTransfFunc(int16_t freq, double control_to_out, int16_t frq_ea_zero, int16_t res_ea)
{
    double num = 1+((2*M_PI*freq)/coTransfZero(control_to_out, frq_ea_zero, res_ea));
    double dnm = 1+((2*M_PI*freq)/coTransfPoleOne());
    return coOptoTransfGain()*(num/dnm);
}

/**
 * @brief coGainOptoFeedbTransfFunc -
 * @param freq
 * @param control_to_out
 * @param frq_ea_zero
 * @param res_ea
 * @return
 */
inline double FCCD::coGainOptoFeedbTransfFunc(int16_t freq, double control_to_out, int16_t frq_ea_zero, int16_t res_ea)
{
    double num = 1+((2*M_PI*freq)/coTransfZero(control_to_out, frq_ea_zero, res_ea));
    double dnm = 1+((2*M_PI*freq)/coTransfPoleOne());
    return 20*std::log10(coOptoTransfGain()*(num/dnm));
}

/**
 * @brief coPhaseOptoFeedbTransfFunc
 * @param freq
 * @param control_to_out
 * @param frq_ea_zero
 * @param res_ea
 * @return
 */
inline double FCCD::coPhaseOptoFeedbTransfFunc(int16_t freq, double control_to_out, int16_t frq_ea_zero, int16_t res_ea)
{
    double num = ((2*M_PI*freq)/coTransfZero(control_to_out, frq_ea_zero, res_ea));
    double dnm = ((2*M_PI*freq)/coTransfPoleOne());
    return qAtan(num)-qAtan(dnm);
}
