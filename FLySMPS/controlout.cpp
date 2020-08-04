#include "controlout.h"

/**
 * @brief coZeroTwoAngFreq
 * @return
 */
inline double PCSSM::coDCMZeroTwoAngFreq() const
{
    double tmp = std::pow(turnrat, 2)*static_cast<double>(resload);
    return tmp/(priminduct*voltrat*(voltrat+1));
}
/**
 * @brief coZeroOneAngFreq
 * @return
 */
inline double PCSSM::coZeroOneAngFreq() const
{
    return 1/(capout*esrcap);
}
/**
 * @brief coPoleTwoAngFreq
 * @return
 */
inline double PCSSM::coDCMPoleTwoAngFreq() const
{
    return (std::pow(turnrat,2)*static_cast<double>(resload))/(priminduct*std::pow((voltrat+1),2));
}
/**
 * @brief coPoleOneAngFreq
 * @return
 */
inline double PCSSM::coPoleOneAngFreq() const
{
    return 2/(static_cast<double>(resload)*capout);
}
/**
 * @brief coGainZero
 * @param duty
 * @param fsw
 * @param vs
 * @return
 */
inline double PCSSM::coDCMGainZero(double duty, float fsw, float vs) const
{
    double tmp = duty/static_cast<double>(vs);
    double load = std::sqrt(static_cast<double>(resload)/(2.*priminduct*static_cast<double>(fsw)));
    return tmp*voltin*load;
}
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
/**
 * @brief coDutyToOutTrasfFunct - G_vd(s) -
 * @param s
 * @param rsense
 * @param fsw
 * @return
 */
inline double PCSSM::coDutyToOutTrasfFunct(double s, double rsense, float fsw)
{
    return coGainCurrModeContrModulator(rsense, fsw)*(((1+(s/coZeroOneAngFreq()))*(1+(s/coDCMZeroTwoAngFreq())))/(1+(s/coPoleOneAngFreq()))*(1+(s/coDCMPoleTwoAngFreq())));
}
/**
 * @brief coControlToOutTransfFunct - G_vc(s) -
 * @param s
 * @param rsense
 * @param fsw
 * @return
 */
inline double PCSSM::coControlToOutTransfFunct(double s, double rsense, float fsw)
{
    return coDutyToOutTrasfFunct(s, rsense, fsw)*coGainCurrModeContrModulator(rsense, fsw);
}
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
