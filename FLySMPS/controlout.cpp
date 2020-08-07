#include "controlout.h"

/**
 * @brief coZeroOneAngFreq - $\omega_{zc}$ - esr zero, lhp
 * @return
 */
inline double PCSSM::coZeroOneAngFreq() const
{
    return 1/(capout*esrcap);
}

/**
 * @brief coPoleOneAngFreq - $\omega_{0}$ the dominant pole
 * @return
 */
inline double PCSSM::coPoleOneAngFreq() const
{
    return 2/(static_cast<double>(resload)*capout);
}
/**********************DCM************************/
/**
 * @brief coZeroTwoAngFreq - right half plane-zero
 * @return
 */
inline double PCSSM::coDCMZeroTwoAngFreq() const
{
    double tmp = std::pow(turnrat, 2)*static_cast<double>(resload);
    return tmp/(priminduct*voltrat*(voltrat+1));
}

/**
 * @brief coPoleTwoAngFreq - right half plane-pole
 * @return
 */
inline double PCSSM::coDCMPoleTwoAngFreq() const
{
    return (std::pow(turnrat,2)*static_cast<double>(resload))/(priminduct*std::pow((voltrat+1),2));
}

/**
 * @brief coDCMCriticValue - K_vd -
 * @param fsw
 * @return
 */
inline double PCSSM::coDCMCriticValue(float fsw) const
{
    double ktmp = (2.*priminduct*static_cast<double>(fsw))/static_cast<double>(resload);
    return static_cast<double>(voltin)/(static_cast<double>(turnrat)*std::sqrt(ktmp));
}
/***********************F_m************************/
/**
 * @brief coCurrDetectSlopeVolt - S_n - the voltage slope when the primary-side current is detected on RS_s
 * @param rsense - the value of current sense resistor
 * @return
 */
inline double PCSSM::coCurrDetectSlopeVolt(double rsense) const
{
    return (voltin*rsense)/priminduct;
}

/**
 * @brief coTimeConst - \tau_L - switching period
 * @param fsw
 * @return
 */
inline double PCSSM::coTimeConst(float fsw) const
{
    return (2*priminduct*static_cast<double>(fsw))/(std::pow(turnrat,2)*static_cast<double>(resload));
}

/**
 * @brief coGainCurrModeContrModulator - F_m - the PWM modulator gain
 * @param rsense - the value of current sense resistor
 * @param fsw - the switch frequency
 * @return
 */
inline double PCSSM::coGainCurrModeContrModulator(double rsense, float fsw) const
{
    return 1/((coCurrDetectSlopeVolt(rsense)+sawvolt)*coTimeConst(fsw));
}
/**********************OUT*************************/
/**
 * @brief coDutyToOutTrasfFunct - G_vd(s) -
 * @param s
 * @param rsense
 * @param fsw
 * @param mode
 * @return
 */
inline double PCSSM::coDutyToOutTrasfFunct(double s, float fsw, double duty, PS_MODE mode)
{
    double result = 0.0;
    double num1 = 0.0;
    double num2 = 0.0;
    double dnm1 = 0.0;
    double dnm2 = 0.0;
    if(mode == DCM_MODE)
    {
        num1 = 1+(s/coZeroOneAngFreq());
        num2 = 1-(s/coDCMZeroTwoAngFreq());
        dnm1 = 1+(s/coPoleOneAngFreq());
        dnm2 = 1+(s/coDCMPoleTwoAngFreq());
        result = coDCMCriticValue(fsw)*((num1*num2)/(dnm1*dnm2));
    }
    else if(mode == CCM_MODE)
    {
        num1 = 1-(s/coCCMZeroTwoAngFreq(duty));
        num2 = 1+(s/coZeroOneAngFreq());
        dnm1 = s/(coCCMQualityFact(duty)*coCCMPoleTwoAngFreq(duty));
        dnm2 = std::pow((s/coCCMPoleTwoAngFreq(duty)),2);
        result = coCCMVoltGainCoeff(duty)*((num1*num2)/(1+dnm1+dnm2));
    }
    else
    {
        return -1; //it's very bad way, i will rewrite this after
    }
    return result;
}

/**
 * @brief coControlToOutTransfFunct - G_vc(s) - The control-to-output transfer function
 * @param s
 * @param rsense
 * @param fsw
 * @return
 */
inline double PCSSM::coControlToOutTransfFunct(double s, double rsense, float fsw, double duty, PS_MODE mode)
{
    double result = 0.0;
    double dnm = 0.0;
    if(mode == DCM_MODE)
    {
        result = coDutyToOutTrasfFunct(s, fsw, duty, mode)*coGainCurrModeContrModulator(rsense, fsw);
    }
    else if(mode == CCM_MODE)
    {
        dnm = coGainCurrModeContrModulator(rsense, fsw)*rsense*coCCMDutyToInductCurrTrasfFunct(s, duty);
        result = (coGainCurrModeContrModulator(rsense, fsw)*coDutyToOutTrasfFunct(s, fsw, duty, mode))/(1+dnm);
    }
    else
    {
        return -1;
    }
    return result;
}
/**********************CCM****************************/
/**
 * @brief coDCMZeroTwoAngFreq - \omega_zrhp -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMZeroTwoAngFreq(double duty) const
{
    double tmp = std::pow(turnrat,2)*std::pow((1-duty),2)*static_cast<double>(resload);
    return tmp/(duty*priminduct);
}

/**
 * @brief coDCMPoleTwoAngFreq - \omega_o -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMPoleTwoAngFreq(double duty) const
{
    double tmp = static_cast<double>(turnrat)/(std::sqrt(capout*priminduct));
    double num = std::pow((1-duty),2);
    double ld = std::sqrt((num*static_cast<double>(resload))/(static_cast<double>(resload)+esrcap));
    return tmp*ld;
}

/**
 * @brief coDCMVoltGainCoeff - K_vd -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMVoltGainCoeff(double duty) const
{
    return static_cast<double>(voltin)/(static_cast<double>(turnrat)*(std::pow((1-duty),2)));
}

/**
 * @brief coDCMCurrGainCoeff - K_id -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMCurrGainCoeff(double duty) const
{
    double tmp = 1+(2*duty/(1-duty));
    double dnm = std::pow((1-duty),2);
    double mult = voltin/(static_cast<double>(turnrat)*dnm*static_cast<double>(resload));
    return tmp*mult;
}

/**
 * @brief coDCMQualityFact - Q -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMQualityFact(double duty) const
{
    double dnm1 = priminduct/(std::pow(static_cast<double>(turnrat),2)*std::pow((1-duty),2)*static_cast<double>(resload));
    double dnm2 = esrcap*static_cast<double>(resload);
    return 1/(coCCMPoleTwoAngFreq(duty)*(dnm1+dnm2));
}

/**
 * @brief coDCMDutyToInductCurrTrasfFunct - G_id(s) -
 * @param s
 * @param duty
 * @return
 */
inline double PCSSM::coCCMDutyToInductCurrTrasfFunct(double s, double duty)
{
    double num = 1+(s/coPoleOneAngFreq());
    double dnm = 1+(s/(coCCMQualityFact(duty)*coCCMPoleTwoAngFreq(duty)))+std::pow((s/coCCMPoleTwoAngFreq(duty)),2);
    return coCCMCurrGainCoeff(duty)*(num/dnm);
}
