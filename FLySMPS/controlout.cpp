#include "controlout.h"

/**
 * @brief coZeroTwoAngFreq
 * @return
 */
inline double PCSSM::coZeroTwoAngFreq() const
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
inline double PCSSM::coPoleTwoAngFreq() const
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
inline double PCSSM::coGainZero(double duty, float fsw, float vs) const
{
    double tmp = duty/static_cast<double>(vs);
    double load = std::sqrt(static_cast<double>(resload)/(2.*priminduct*static_cast<double>(fsw)));
    return tmp*voltin*load;
}
/**
 * @brief coCurrDetectSlopeVolt
 * @param rsense
 * @return
 */
inline double PCSSM::coCurrDetectSlopeVolt(double rsense) const
{
    return (voltin*rsense)/priminduct;
}
/**
 * @brief coTimeConst
 * @param fsw
 * @return
 */
inline double PCSSM::coTimeConst(float fsw) const
{
    return (2*priminduct*static_cast<double>(fsw))/(std::pow(turnrat,2)*static_cast<double>(resload));
}
/**
 * @brief coGainCurrModeContrModulator
 * @param rsense
 * @param fsw
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
inline double PCSSM::coCntrlToOutTrasfFunct(double s, double rsense, float fsw)
{
    return coGainCurrModeContrModulator(rsense, fsw)*(((1+(s/coZeroOneAngFreq()))*(1+(s/coZeroTwoAngFreq())))/(1+(s/coPoleOneAngFreq()))*(1+(s/coPoleTwoAngFreq())));
}
