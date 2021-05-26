#include "controlout.h"

/********************COM*************************/
/**
 * @brief coZeroOneAngFreq - $\f_{zc}$ - esr zero, lhp
 * @return esr zero value, angular frequency
 */
inline double PCSSM::coZeroOneAngFreq() const
{
    return 1/(2 * M_PI * m_ssmvar.output_cap * m_ssmvar.output_cap_esr);
}

/**
 * @brief coPoleOneAngFreq - $\f_{rc}$ the dominant pole
 * @return
 */
inline double PCSSM::coPoleOneAngFreq() const
{
    return 2/(2 * M_PI * static_cast<double>(m_ssmvar.output_full_load_res) * m_ssmvar.output_cap);
}
/**********************DCM************************/
/**
 * @brief coZeroTwoAngFreq - $\f_{zrhp}$ - rhp zero
 * @return
 */
inline double PCSSM::coDCMZeroTwoAngFreq() const
{
    double voltrat = m_ssmvar.output_voltage/m_ssmvar.input_voltage;
    double tmp = qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res);

    return tmp/(2 * M_PI * m_ssmvar.primary_ind * voltrat*(voltrat+1));
}

/**
 * @brief coPoleTwoAngFreq - $\f_{p2}$ - pole
 * @return
 */
inline double PCSSM::coDCMPoleTwoAngFreq() const
{
    double voltrat = m_ssmvar.output_voltage/m_ssmvar.input_voltage;

    return (qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res))
            /(2 * M_PI * m_ssmvar.primary_ind * qPow((voltrat+1),2));
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
    return 1/((coCurrDetectSlopeVolt()+m_ssmvar.sawvolt)*coTimeConst());
}
/**********************OUT*************************/
/**
 * @brief coMagDutyToOutTrasfFunct - $G_{vd}(s)$ -
 * @param freq
 * @return
 */
double PCSSM::coMagDutyToOutTrasfFunct(int32_t freq)
{
    double result = 0.0;
    double num1 = 0.0;
    double num2 = 0.0;
    double dnm1 = 0.0;
    double dnm2 = 0.0;
    if(m_mode == DCM_MODE)
    {
        num1 = qSqrt(1 + qPow((freq/coZeroOneAngFreq()), 2));
        num2 = qSqrt(1 + qPow((freq/coDCMZeroTwoAngFreq()), 2));
        dnm1 = qSqrt(1 + qPow((freq/coPoleOneAngFreq()), 2));
        dnm2 = qSqrt(1 + qPow((freq/coDCMPoleTwoAngFreq()), 2));
        result = 20 * log10(coDCMCriticValue()*((num1*num2)/(dnm1*dnm2)));
    }
    else if(m_mode == CCM_MODE)
    {
        num1 = qSqrt(1 + qPow((freq/coCCMZeroTwoAngFreq()), 2));
        num2 = qSqrt(1 + qPow((freq/coZeroOneAngFreq()), 2));
        dnm1 = qSqrt(
                    qPow(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2), 2) +
                    qPow((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())), 2)
                    );

        result = 20 * log10(coCCMVoltGainCoeff()*((num1*num2)/(dnm1)));
    }
    return result;
}

/**
 * @brief coPhsDutyToOutTrasfFunct - $G_{vd}(s)$ -
 * @param freq
 * @return
 */
double PCSSM::coPhsDutyToOutTrasfFunct(int32_t freq)
{
    double result = 0.0;
    double farg = qAtan(freq/coCCMZeroTwoAngFreq());
    double sarg = qAtan(freq/coZeroOneAngFreq());
    double targ = 0.;
    if(m_mode == DCM_MODE)
    {
        targ = qAtan(freq/coPoleOneAngFreq());
        result = farg + sarg - targ + qAtan(freq/coDCMPoleTwoAngFreq());
    }
    else if(m_mode == CCM_MODE)
    {
        targ = qAtan((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())) * (1/(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2))));
        result = farg + sarg - targ;
    }
    return result;
}

/**
 * @brief coMagControlToOutTransfFunct - $G_{vc}(s)$ - The control-to-output transfer function
 * @param s
 * @return
 */
double PCSSM::coMagControlToOutTransfFunct(int32_t freq)
{
    double result = 0.0;
    double dnm = 0.0;
    if(m_mode == DCM_MODE)
    {
        result = coMagDutyToOutTrasfFunct(freq) * (20 * log10(coGainCurrModeContrModulator()));
    }
    else if(m_mode == CCM_MODE)
    {
        dnm = 20 * log10(coGainCurrModeContrModulator() * m_ssmvar.res_sense) * coMagCCMDutyToInductCurrTrasfFunct(freq);
        result = ( 20 * log10(coGainCurrModeContrModulator()) * coMagDutyToOutTrasfFunct(freq))/(1+dnm);
    }
    return result;

}

double PCSSM::coPhsControlToOutTransfFunct(int32_t freq)
{
    double result = 0.0;
    double dnm = 0.0;
    if(m_mode == DCM_MODE)
    {
        result = coPhsDutyToOutTrasfFunct(freq) * qAtan(coGainCurrModeContrModulator());
    }
    else if(m_mode == CCM_MODE)
    {
        dnm = qAtan(coGainCurrModeContrModulator() * m_ssmvar.res_sense) * coMagCCMDutyToInductCurrTrasfFunct(freq);
        result = (qAtan(coGainCurrModeContrModulator()) * coMagDutyToOutTrasfFunct(freq))/(1+dnm);
    }
    return result;

}

/**********************CCM****************************/
/**
 * @brief coCCMZeroTwoAngFreq - $f_{zrhp}$
 * @return
 */
inline double PCSSM::coCCMZeroTwoAngFreq() const
{
    double tmp = qPow(m_ssmvar.turn_ratio, 2) * qPow((1-m_ssmvar.actual_duty), 2)
            * static_cast<double>(m_ssmvar.output_full_load_res);

    return tmp/(2 * M_PI * m_ssmvar.actual_duty * m_ssmvar.primary_ind);
}

/**
 * @brief coCCMPoleTwoAngFreq - $f_{o}$
 * @return
 */
inline double PCSSM::coCCMPoleTwoAngFreq() const
{
    double tmp = static_cast<double>(m_ssmvar.turn_ratio)
            /(2 * M_PI * qSqrt(m_ssmvar.output_cap * m_ssmvar.primary_ind));

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
 * @brief coDCMDutyToInductCurrTrasfFunct - $G_{id}(s) magnitude value for current frequency$
 * @param freq
 * @return
 */
double PCSSM::coMagCCMDutyToInductCurrTrasfFunct(int32_t freq)
{
    double num = qSqrt(1 + qPow((freq/coPoleOneAngFreq()), 2));

    double dnm = qSqrt(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2) + qPow((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())), 2));

    return coCCMCurrGainCoeff() * (num/dnm);
}

/**
 * @brief coPhsCCMDutyToInductCurrTrasfFunct - $G_{id}(s) phase value of current frequency$
 * @param freq
 * @return
 */
double PCSSM::coPhsCCMDutyToInductCurrTrasfFunct(int32_t freq)
{
    double farg = qAtan(freq/coPoleOneAngFreq());
    double sarg = qAtan(
                        (freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())) *
                        (1/(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2)))
                       );
    return farg - sarg;
}

void PCSSM::coGainOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_mag)
{
    auto itr_freq = in_freq.begin();
    out_mag.reserve(in_freq.size());
    double frq = 0., result = 0.;

    while(itr_freq != in_freq.end())
    {
        frq = *itr_freq;
        result = coMagControlToOutTransfFunct(frq);
        out_mag.push_back(result);
        itr_freq++;
    }
}

void PCSSM::coPhaseOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_phase)
{
    auto itr_freq = in_freq.begin();
    out_phase.reserve(in_freq.size());
    double frq = 0., result = 0.;

    while(itr_freq != in_freq.end())
    {
        frq = *itr_freq;
        result = coPhsControlToOutTransfFunct(frq);
        out_phase.push_back(result);
        itr_freq++;
    }
}

/**************************FCCD*************************/
/**
 * @brief coResOptoDiode - R_{LED_max}
 * @return
 */
inline int32_t FCCD::coResOptoDiode() const
{
    double num = m_fcvar.out_voltage - S_OPTO_FORVARD_DROP - S_TL431_VREF;
    
    double dnm = S_INT_BIAS_CONTR - S_OPTO_CE_SAT +
            (S_TL431_CURR_CATH * m_fcvar.opto_ctr * m_fcvar.res_pull_up);
    
    return static_cast<int16_t>((num/dnm)*0.15);//15% marg
}

/**
 * @brief coResOptoBias - R_{bias}
 * @return
 */
inline int32_t FCCD::coResOptoBias() const
{
    double num = S_OPTO_FORVARD_DROP+(coResOptoDiode()*((S_INT_BIAS_CONTR-3)/
                                                        (m_fcvar.opto_ctr * m_fcvar.res_pull_up)));
    return static_cast<int16_t>(num / S_TL431_CURR_CATH);
}

/**
 * @brief coResUp - R_{1} or R_{up} in voltage divider K_{d}
 * @return
 */
inline int32_t FCCD::coResUp() const
{
    return m_fcvar.res_down * static_cast<int16_t>((m_fcvar.out_voltage - S_TL431_VREF)/S_TL431_VREF);
}

/**
 * @brief coVoltageDivideGain - K_{d}
 * @return
 */
inline double FCCD::coVoltageDivideGain() const
{
    return m_fcvar.res_down/(coResUp() + m_fcvar.res_down);
}

/**
 * @brief coVoltageOptoGain - K_{c} or G_{1} in Db 20*log10(K_{c})
 * @return
 */
inline double FCCD::coVoltageOptoGain() const
{
    return (m_fcvar.res_pull_up/coResOptoDiode()) * m_fcvar.opto_ctr;
}

/**
 * @brief FCCD::coIndOnTimeSlope - S_{n}
 * @return
 */
inline double FCCD::coIndOnTimeSlope() const
{
    return (m_rsvar.inp_voltage * m_rsvar.res_sense)/m_rsvar.primary_ind;
}

/**
 * @brief FCCD::coCompRamp - M_{c}
 * @return
 */
inline double FCCD::coCompRamp() const
{
    double coeff = m_rsvar.prim_turns/m_rsvar.sec_turns_to_control;

    return (coeff * m_fcvar.out_voltage)/m_rsvar.inp_voltage;
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
    double inv_duty = 1 - m_rsvar.actual_duty;
    return 1/(M_PI*(coCompRamp()*inv_duty-0.5));
}

/**
 * @brief coFreqCrossSection - f_{cross}
 * @return
 */
inline double FCCD::coFreqCrossSection() const
{
    double ratio_factor = qPow((m_rsvar.prim_turns/m_rsvar.sec_turns_to_control), 2);

    double coeff = 1/5;

    double num = (qPow(m_fcvar.out_voltage, 2)/
                  m_rsvar.out_pwr_tot) * qPow(m_rsvar.actual_duty, 2);

    double dnm = 2 * M_PI * m_rsvar.primary_ind;

    return (num/dnm)*ratio_factor*coeff;
}

/**
 * @brief coFreqPole - f_{pole}
 * @return
 */
inline double FCCD::coFreqPole() const
{
    return (qTan(coBoost()) + qSqrt(qPow(qTan(coBoost()), 2) + 1)) * coFreqCrossSection();
}

/**
 * @brief coFreqZero - f_{zero}
 * @return
 */
inline double FCCD::coFreqZero() const
{
    return qPow(coFreqCrossSection(), 2)/coFreqPole();
}

/**
 * @brief coResErrorAmp -
 * @return
 */
int32_t FCCD::coResZero() const
{
    double gplant = 0.;
    double pz_ratio = coFreqPole()/coFreqZero();
    double t_ratio = m_rsvar.sec_turns_to_control/m_rsvar.prim_turns;
    double ind_coeff = m_rsvar.inp_voltage * m_rsvar.res_sense * m_rsvar.primary_ind;

    if(m_mode == DCM_MODE){
        gplant = pz_ratio * qSqrt((m_rsvar.primary_ind * m_fcvar.freq_sw * m_fcvar.out_voltage)/(8 * m_fcvar.out_current)) *
                ((m_rsvar.inp_voltage)/ind_coeff);
    }
    else if(m_mode == CCM_MODE){
        gplant = pz_ratio * ((qPow(m_rsvar.inp_voltage, 2) * m_fcvar.out_voltage) /
                          (2 * m_fcvar.out_current * (2 * m_rsvar.inp_voltage + t_ratio * m_fcvar.out_voltage) * ind_coeff));
    }

    double coeff = qSqrt(qPow((coFreqCrossSection()/coFreqPole()), 2) + 1)/
                   qSqrt(qPow((coFreqZero()/coFreqCrossSection()), 2) + 1);

    return gplant * coResUp() * coeff;

}
/**
 * @brief coCapZero - C_{zero}
 * @return
 */
inline double FCCD::coCapZero() const
{
    return 1/(2 * M_PI * coFreqZero() * coResZero());
}

/**
 * @brief coCapPoleOpto - C_{opto}
 * @return
 */
inline double FCCD::coCapPoleOpto() const
{
    return 1/(2 * M_PI * coFreqZero() * coResZero());
}

/**
 * @brief coTransfLCZero - \omega_{lc-z}
 * @return
 */
inline double FCCD::coTransfLCZero() const
{
    double lc_ratio = 1/qSqrt(m_lcfvar.lcf_ind * m_lcfvar.lcf_cap);
    double load_ratio = qSqrt((m_fcvar.out_voltage/m_fcvar.out_current)/(m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current)));
    return lc_ratio * load_ratio;
}

/**
 * @brief coQualityLC - Q_{lc}
 * @return
 */
inline double FCCD::coQualityLC() const
{
    double num = m_lcfvar.lcf_ind * m_lcfvar.lcf_cap * coTransfLCZero() * (m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current));
    double dnm = m_lcfvar.lcf_ind + m_lcfvar.lcf_cap * (m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current));

    return num/dnm;
}

/**
 * @brief coTransfZero - $\omega_{z}$
 * @return
 */
inline double FCCD::coTransfZero() const
{
    return 1/(coResZero() * coCapZero());
}

/**
 * @brief coTransfPoleOne - $\omega_{p1}$
 * @return
 */
inline double FCCD::coTransfPoleOne() const
{
    return 1/(m_fcvar.res_pull_up * coCapPoleOpto());
}

/**
 * @brief coLCTransfFunc - H_{lc}(s)
 * @param s
 * @return
 */
inline double FCCD::coMagLCTransfFunc(int32_t freq) const
{
    double qual = qPow((freq/coQualityLC() * coTransfLCZero()), 2);
    double zero = qPow(1 - qPow((freq/coTransfLCZero()), 2), 2);

    double dnm = qSqrt(zero + qual);

    return 1/dnm;
}

/**
 * @brief coOptoFeedbTransfFunc - H(s)
 * @param s
 * @return
 */
inline double FCCD::coMagOptoFeedbTransfFunc(int32_t freq) const
{
    double zero_one = qSqrt(1 + qPow((freq/coTransfZero()), 2));
    double pole_one = qSqrt(1 + qPow((freq/coTransfPoleOne()), 2));
    double g_0 = coVoltageOptoGain() * coVoltageDivideGain() * (coResZero()/coResUp());

    return g_0 * (zero_one/pole_one) * coMagLCTransfFunc(freq);
}

inline double FCCD::coPhsLCTransfFunc(int32_t freq) const
{
    return qAtan((freq/(coTransfLCZero() * coQualityLC())) * (1/(1 - qPow((freq/coTransfLCZero()), 2))));
}

inline double FCCD::coPhsOptoFeedbTransfFunc(int32_t freq) const
{
    return qAtan(freq/coTransfZero())-qAtan(freq/coTransfPoleOne());
}

/**
 * @brief coGainOptoFeedbTransfFunc -
 * @param freq
 * @return
 */
void FCCD::coGainOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_mag)
{
    auto itr_freq = in_freq.begin();
    out_mag.reserve(in_freq.size());
    double frq = 0., result = 0.;

    while(itr_freq != in_freq.end())
    {
        frq = *itr_freq;
        result = 20 * log10(coMagOptoFeedbTransfFunc(frq));
        out_mag.push_back(result);
        itr_freq++;
    }
}

/**
 * @brief coPhaseOptoFeedbTransfFunc
 * @param freq
 * @return
 */
void FCCD::coPhaseOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_phase)
{
    auto itr_freq = in_freq.begin();
    out_phase.reserve(in_freq.size());
    double frq = 0., result = 0.;

    while (itr_freq != in_freq.end())
    {
        frq = *itr_freq;
        result = coPhsOptoFeedbTransfFunc(frq) - coPhsLCTransfFunc(frq);
        out_phase.push_back(result);
        itr_freq++;
    }
}
