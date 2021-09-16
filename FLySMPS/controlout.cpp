#include "controlout.h"

PCSSM::PCSSM(SSMPreDesign &ssmvar, PS_MODE mode)
{
    qSwap(m_ssmvar, ssmvar);
    m_mode = mode;
}

inline double PCSSM::coZeroOneAngFreq() const
{
    return 1/(m_ssmvar.output_cap * m_ssmvar.output_cap_esr);
}

inline double PCSSM::coPoleOneAngFreq() const
{
    return 2/(static_cast<double>(m_ssmvar.output_full_load_res) * m_ssmvar.output_cap);
}

inline double PCSSM::coDCMZeroTwoAngFreq() const
{
    double voltrat = static_cast<double>(m_ssmvar.output_voltage)/m_ssmvar.input_voltage;
    double tmp = qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res);

    return tmp/(m_ssmvar.primary_ind * voltrat*(voltrat+1));
}

inline double PCSSM::coDCMPoleTwoAngFreq() const
{
    double voltrat = static_cast<double>(m_ssmvar.output_voltage)/m_ssmvar.input_voltage;

    return (qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res))
            /(m_ssmvar.primary_ind * qPow((voltrat+1),2));
}

inline double PCSSM::coDCMCriticValue() const
{
    double ktmp = (2. * m_ssmvar.primary_ind * static_cast<double>(m_ssmvar.freq_switch))
            /static_cast<double>(m_ssmvar.output_full_load_res);

    return static_cast<double>(m_ssmvar.input_voltage)
            /(static_cast<double>(m_ssmvar.turn_ratio) * qSqrt(ktmp));
}

inline double PCSSM::coCCMZeroTwoAngFreq() const
{
    double tmp = qPow(m_ssmvar.turn_ratio, 2) * qPow((1-m_ssmvar.actual_duty), 2)
            * static_cast<double>(m_ssmvar.output_full_load_res);

    return tmp/(2 * M_PI * m_ssmvar.actual_duty * m_ssmvar.primary_ind);
}

inline double PCSSM::coCCMPoleTwoAngFreq() const
{
    double tmp = static_cast<double>(m_ssmvar.turn_ratio)
            /(2 * M_PI * qSqrt(m_ssmvar.output_cap * m_ssmvar.primary_ind));

    double num = qPow((1-m_ssmvar.actual_duty), 2);

    double ld = qSqrt((num*static_cast<double>(m_ssmvar.output_full_load_res))
                      /(static_cast<double>(m_ssmvar.output_full_load_res)+m_ssmvar.output_cap_esr));
    return tmp*ld;
}

inline double PCSSM::coCCMVoltGainCoeff() const
{
    return static_cast<double>(m_ssmvar.input_voltage)
            /(static_cast<double>(m_ssmvar.turn_ratio) * (qPow((1-m_ssmvar.actual_duty),2)));
}

inline double PCSSM::coCCMCurrGainCoeff() const
{
    double tmp = 1.+(2.*m_ssmvar.actual_duty/(1.-m_ssmvar.actual_duty));

    double dnm = qPow((1-m_ssmvar.actual_duty), 2);

    double mult = m_ssmvar.input_voltage
            /(static_cast<double>(m_ssmvar.turn_ratio) * dnm * static_cast<double>(m_ssmvar.output_full_load_res));

    return tmp*mult;
}

inline double PCSSM::coCCMQualityFact() const
{
    double dnm1 = m_ssmvar.primary_ind
            /(qPow(static_cast<double>(m_ssmvar.turn_ratio), 2)
            * qPow((1-m_ssmvar.actual_duty), 2) * static_cast<double>(m_ssmvar.output_full_load_res));

    double dnm2 = m_ssmvar.output_cap_esr * static_cast<double>(m_ssmvar.output_full_load_res);

    return 1/(coCCMPoleTwoAngFreq()*(dnm1+dnm2));
}

double PCSSM::coMagCCMDutyToInductCurrTrasfFunct(const double freq)
{
    double num = qSqrt(1 + qPow((freq/coPoleOneAngFreq()), 2));

    double dnm = qSqrt(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2) + qPow((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())), 2));

    return coCCMCurrGainCoeff() * (num/dnm);
}

double PCSSM::coPhsCCMDutyToInductCurrTrasfFunct(const double freq)
{
    double farg = qAtan(freq/coPoleOneAngFreq());
    double sarg = qAtan(
                        (freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())) *
                        (1/(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2)))
                       );
    return farg - sarg;
}

inline double PCSSM::coCurrDetectSlopeVolt() const
{
    return (m_ssmvar.input_voltage * m_ssmvar.res_sense)
            / m_ssmvar.primary_ind;
}

inline double PCSSM::coTimeConst() const
{
    return (2 * m_ssmvar.primary_ind * static_cast<double>(m_ssmvar.freq_switch))
            /(qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res));
}

inline double PCSSM::coGainCurrModeContrModulator() const
{
    return 1/((coCurrDetectSlopeVolt()+m_ssmvar.sawvolt)*coTimeConst());
}

double PCSSM::coMagDutyToOutTrasfFunct(const double freq)
{
    double result = 0.0;
    double num1 = 0.0;
    double num2 = 0.0;
    double dnm1 = 0.0;
    double dnm2 = 0.0;
    if(m_mode == DCM_MODE)
    {
        num1 = qSqrt(1 + qPow((freq/coZeroOneAngFreq()), 2));
        num2 = qSqrt(1 - qPow((freq/coDCMZeroTwoAngFreq()), 2));
        dnm1 = qSqrt(1 + qPow((freq/coPoleOneAngFreq()), 2));
        dnm2 = qSqrt(1 + qPow((freq/coDCMPoleTwoAngFreq()), 2));
        result = coDCMCriticValue()*((num1*num2)/(dnm1*dnm2));
    }
    else if(m_mode == CCM_MODE)
    {
        num1 = qSqrt(1 + qPow((freq/coCCMZeroTwoAngFreq()), 2));
        num2 = qSqrt(1 + qPow((freq/coZeroOneAngFreq()), 2));
        dnm1 = qSqrt(
                    qPow(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2), 2) +
                    qPow((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())), 2)
                    );
        result = coCCMVoltGainCoeff()*((num1*num2)/(dnm1));
    }
    return result;
}

double PCSSM::coPhsDutyToOutTrasfFunct(const double freq)
{
    double result = 0.0;
    double farg = qAtan(freq/coCCMZeroTwoAngFreq());
    double sarg = qAtan(freq/coZeroOneAngFreq());
    double targ = 0.;
    if(m_mode == DCM_MODE)
    {
        targ = qAtan(freq/coPoleOneAngFreq());
        result = farg - sarg - targ - qAtan(freq/coDCMPoleTwoAngFreq());
    }
    else if(m_mode == CCM_MODE)
    {
        targ = qAtan((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())) * (1/(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2))));
        result = farg - sarg - targ;
    }
    return result;
}

double PCSSM::coMagControlToOutTransfFunct(const double freq)
{
    double result = 0.0;
    double dnm = 0.0;
    if(m_mode == DCM_MODE)
    {
        result = 20 * log10(coMagDutyToOutTrasfFunct(freq) * coGainCurrModeContrModulator());
    }
    else if(m_mode == CCM_MODE)
    {
        dnm = 20 * log10(coGainCurrModeContrModulator() * m_ssmvar.res_sense) * coMagCCMDutyToInductCurrTrasfFunct(freq);
        result = ( 20 * log10(coGainCurrModeContrModulator()) * coMagDutyToOutTrasfFunct(freq))/(1+dnm);
    }
    return result;

}

double PCSSM::coPhsControlToOutTransfFunct(const double freq)
{
    double result = 0.0;
    double dnm = 0.0;
    if(m_mode == DCM_MODE)
    {
        result = coPhsDutyToOutTrasfFunct(freq) * (M_PI_DEG/M_PI);
    }
    else if(m_mode == CCM_MODE)
    {
        dnm = qAtan(coGainCurrModeContrModulator() * m_ssmvar.res_sense) * coMagCCMDutyToInductCurrTrasfFunct(freq);
        result = (qAtan(coGainCurrModeContrModulator()) * coMagDutyToOutTrasfFunct(freq))/(1+dnm);
    }
    return result;

}

void PCSSM::coGainControlToOutTransfFunct(QVector<double> &in_freq, QVector<double> &out_mag)
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
    emit arrayMagSSMComplete();
}

void PCSSM::coPhaseControlToOutTransfFunct(QVector<double> &in_freq, QVector<double> &out_phase)
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
    emit arrayPhaseSSMComplete();
}

FCCD::FCCD(FCPreDesign &fcvar, RampSlopePreDesign &rsvar, LCSecondStage &lcfvar, PS_MODE mode)
{
    qSwap(m_fcvar, fcvar);
    qSwap(m_rsvar, rsvar);
    qSwap(m_lcfvar, lcfvar);
    m_mode = mode;
}

double FCCD::coFreqCrossSection() const
{
    double ratio_factor = qPow((m_rsvar.prim_turns/m_rsvar.sec_turns_to_control), 2);

    double coeff = 1./5.;

    double num = (qPow(m_fcvar.out_voltage, 2)/
                  m_rsvar.out_pwr_tot) * qPow(m_rsvar.actual_duty, 2);

    double dnm = 2 * M_PI * m_rsvar.primary_ind;

    return (num/dnm)*ratio_factor*coeff;
}

inline double FCCD::coFreqPole() const
{
    return (qTan(coBoost()) + qSqrt(qPow(qTan(coBoost()), 2) + 1)) * coFreqCrossSection();
}

inline double FCCD::coFreqZero() const
{
    return qPow(coFreqCrossSection(), 2)/coFreqPole();
}

double FCCD::coResOptoDiode() const
{
    double num = static_cast<double>(m_fcvar.out_voltage) - S_OPTO_FORVARD_DROP - S_TL431_VREF;

    double dnm = S_INT_BIAS_CONTR - S_OPTO_CE_SAT + (S_TL431_CURR_CATH * m_fcvar.opto_ctr * m_fcvar.res_pull_up);

    return (num / dnm) * (m_fcvar.opto_ctr * m_fcvar.res_pull_up) * 0.15;//15% marg
}

inline double FCCD::coResUp() const
{
    return m_fcvar.res_down * (m_fcvar.out_voltage - S_TL431_VREF) / S_TL431_VREF;
}

//3.
/**
 * @brief coResOptoBias - R_{bias}
 * @return
 */
double FCCD::coResOptoBias() const
{
    double num = S_OPTO_FORVARD_DROP + (coResOptoDiode() * ((S_INT_BIAS_CONTR - 3)/
                                                        (m_fcvar.opto_ctr * m_fcvar.res_pull_up)));
    return num / (S_TL431_CURR_CATH);
}

//4.
/**
 * @brief coVoltageOptoGain - K_{c} or G_{0} in Db 20*log10(K_{c})
 * @return
 */
inline double FCCD::coVoltageOptoGain() const
{
    return (m_fcvar.res_pull_up/coResOptoDiode()) * m_fcvar.opto_ctr;
}

double FCCD::coResZero() const
{
    /*
    double gplant = 0.;
    double pz_ratio = coFreqPole()/coFreqZero();
    double t_ratio = static_cast<double>(m_rsvar.sec_turns_to_control)/m_rsvar.prim_turns;
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
    */
    double gain = (1 / m_fcvar.res_pull_up) * (coResOptoDiode() / m_fcvar.opto_ctr);
    double coeff = (gain * coFreqCrossSection() * coResUp()) / coFreqPole();

    double fr_coeff = qSqrt(((qPow(coFreqZero(), 2) + qPow(coFreqCrossSection(), 2)) * (qPow(coFreqPole(), 2) + qPow(coFreqCrossSection(), 2)))) / (qPow(coFreqZero(), 2) + qPow(coFreqCrossSection(), 2));

    return fr_coeff * coeff;
}

inline double FCCD::coCapZero() const
{
    return 1/(2 * M_PI * coFreqZero() * coResZero());
}

inline double FCCD::coCapPoleOpto() const
{
    return 1/(2 * M_PI * coFreqPole() * m_fcvar.res_pull_up);
}

inline double FCCD::coResDivideGain() const
{
    return coResZero()/coResUp();
}

inline double FCCD::coTransfZero() const
{
    return 1/(2 * M_PI * coResZero() * coCapZero());
}

inline double FCCD::coTransfPoleOne() const
{
    return 1/(2 * M_PI * m_fcvar.res_pull_up * coCapPoleOpto());
}

inline double FCCD::coTranfRCZero() const
{
    return 1/(2 * M_PI * m_fcvar.out_sm_cap_esr * m_fcvar.out_sm_cap);
}

inline double FCCD::coTransfLCZero() const
{
    /*
    double lc_ratio = 1/qSqrt(m_lcfvar.lcf_ind * m_lcfvar.lcf_cap);
    double load_ratio = qSqrt((m_fcvar.out_voltage/m_fcvar.out_current)/(m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current)));
    return lc_ratio * load_ratio;
    */
    //return 1/(2 * M_PI * qSqrt(m_lcfvar.lcf_ind * m_lcfvar.lcf_cap));
    return qSqrt((m_lcfvar.lcf_cap_esr + m_lcfvar.lcf_cap) / ( m_lcfvar.lcf_ind * m_lcfvar.lcf_cap_esr * m_lcfvar.lcf_cap));
}

double FCCD::coQualityLC() const
{
    /*
    double num = m_lcfvar.lcf_ind * m_lcfvar.lcf_cap * coTransfLCZero() * (m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current));
    double dnm = m_lcfvar.lcf_ind + m_lcfvar.lcf_cap * (m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current));

    return num/dnm;
    */
    //double r_load = m_fcvar.out_voltage/m_fcvar.out_current;
    //double dnm = (1/r_load) * (qSqrt(m_lcfvar.lcf_ind / m_lcfvar.lcf_cap) + (m_fcvar.out_sm_cap_esr + m_lcfvar.lcf_cap_esr) + qSqrt(m_lcfvar.lcf_cap / m_lcfvar.lcf_ind));

    //return 1 / dnm;
    return (m_fcvar.out_voltage / m_fcvar.out_current) / m_lcfvar.lcf_ind;
}

inline double FCCD::coIndOnTimeSlope() const
{
    return (m_rsvar.inp_voltage * m_rsvar.res_sense)/m_rsvar.primary_ind;
}

double FCCD::coCompRamp() const
{
    double coeff = m_rsvar.prim_turns/m_rsvar.sec_turns_to_control;

    return (coeff  * m_fcvar.out_voltage)/m_rsvar.inp_voltage;
}

inline double FCCD::coExterRampSlope() const
{
    return 0.5 * ((m_fcvar.out_voltage / ((m_rsvar.prim_turns/m_rsvar.sec_turns_to_control) * m_rsvar.primary_ind)) * m_rsvar.res_sense);
    //return (coCompRamp()-1)*coIndOnTimeSlope();
}

double FCCD::coQuality() const
{
    double inv_duty = 1 - m_rsvar.actual_duty;
    return 1/(M_PI*(coCompRamp()*inv_duty-0.5));
}

double FCCD::coMagLCTransfFunc(const double freq) const
{
    //double omega_zero = qPow(1 - qPow((freq/coTransfLCZero()), 2), 2);
    //double qual = qPow((freq/coQualityLC() * coTransfLCZero()), 2);

    double omega_zer1 = qSqrt(1 + qPow((freq/coTranfRCZero()), 2));
    double dnm = qSqrt(qPow((1 - qPow((freq / coTransfLCZero()), 2)), 2) + qPow((freq / (coQualityLC() * coTransfLCZero())), 2));

    return omega_zer1 * (1/dnm);
}

inline double FCCD::coMagOptoFeedbTransfFunc(const double freq) const
{
    double zero_one = qSqrt(1 + qPow((coTransfZero()/freq), 2));
    double pole_one = qSqrt(1 + qPow((freq/coTransfPoleOne()), 2));
    double g_0 = coVoltageOptoGain() * coResDivideGain();

    return g_0 * (zero_one / pole_one) * coMagLCTransfFunc(freq);
}

inline double FCCD::coPhsLCTransfFunc(const double freq) const
{
    return qAtan(freq/coTranfRCZero()) - qAtan((freq/(coTransfLCZero() * coQualityLC())) * (1/(1 - qPow((freq/coTransfLCZero()), 2))));
}

inline double FCCD::coPhsOptoFeedbTransfFunc(const double freq) const
{
    return qAtan(freq/coTransfZero())-qAtan(freq/coTransfPoleOne());
}

void FCCD::coGainOptoFeedbTransfFunc(QVector<double> &in_freq, QVector<double> &out_mag)
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

void FCCD::coPhaseOptoFeedbTransfFunc(QVector<double> &in_freq, QVector<double> &out_phase)
{
    auto itr_freq = in_freq.begin();
    out_phase.reserve(in_freq.size());
    double frq = 0., result = 0.;

    while (itr_freq != in_freq.end())
    {
        frq = *itr_freq;
        result = (coPhsOptoFeedbTransfFunc(frq) - coPhsLCTransfFunc(frq))  * (M_PI_DEG/M_PI);
        out_phase.push_back(result);
        itr_freq++;
    }
}
