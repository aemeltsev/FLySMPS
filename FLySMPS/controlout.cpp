#include "controlout.h"

/**
 * @brief coZeroTwoAngFreq
 * @return
 */
inline double PCSSM::coCCMZeroTwoAngFreq() const
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
inline double PCSSM::coCCMPoleTwoAngFreq() const
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
inline double PCSSM::coCCMGainZero(double duty, float fsw, float vs) const
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
 * @brief coGainCurrModeContrModulator - F_m - the modulator gain
 * @param rsense - the value of current sense resistor
 * @param fsw - the switch frequency
 * @return
 */
inline double PCSSM::coGainCurrModeContrModulator(double rsense, float fsw) const
{
    return 1/((coCurrDetectSlopeVolt(rsense)+sawvolt)*coTimeConst(fsw));
}
/**
 * @brief coDutyToOutTrasfFunct
 * @param s
 * @param rsense
 * @param fsw
 * @return
 */
inline double PCSSM::coDutyToOutTrasfFunct(double s, double rsense, float fsw)
{
    return coGainCurrModeContrModulator(rsense, fsw)*(((1+(s/coZeroOneAngFreq()))*(1+(s/coZeroTwoAngFreq())))/(1+(s/coPoleOneAngFreq()))*(1+(s/coPoleTwoAngFreq())));
}
/**
 * @brief coControlToOutTransfFunct
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
 * @brief coDCMZeroTwoAngFreq
 * @param duty
 * @return
 */
inline double PCSSM::coDCMZeroTwoAngFreq(double duty) const
{
    double tmp = std::pow(turnrat,2)*std::pow((1-duty),2)*static_cast<double>(resload);
    return tmp/(duty*priminduct);
}
/**
 * @brief coDCMPoleTwoAngFreq
 * @param duty
 * @return
 */
inline double PCSSM::coDCMPoleTwoAngFreq(double duty) const
{
    double tmp = static_cast<double>(turnrat)/(std::sqrt(capout*priminduct));
    double num = std::pow((1-duty),2);
    double ld = std::sqrt((num*static_cast<double>(resload))/(static_cast<double>(resload)+esrcap));
    return tmp*ld;
}
/**
 * @brief coDCMVoltGainCoeff
 * @param duty
 * @return
 */
inline double PCSSM::coDCMVoltGainCoeff(double duty) const
{
    return static_cast<double>(voltin)/(static_cast<double>(turnrat)*(std::pow((1-duty),2)));
}
/**
 * @brief coDCMCurrGainCoeff
 * @param duty
 * @return
 */
inline double PCSSM::coDCMCurrGainCoeff(double duty) const
{
    double tmp = 1+(2*duty/(1-duty));
    double dnm = std::pow((1-duty),2);
    double mult = voltin/(static_cast<double>(turnrat)*dnm*static_cast<double>(resload));
    return tmp*mult;
}
