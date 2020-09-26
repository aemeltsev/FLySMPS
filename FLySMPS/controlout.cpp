#include "controlout.h"

/********************COM*************************/
/**
 * @brief coZeroOneAngFreq - $\omega_{zc}$ - esr zero, lhp
 * @return
 */
inline double PCSSM::coZeroOneAngFreq() const
{
    return 1/(capout*esrcap);
}

/**
 * @brief coPoleOneAngFreq - $\omega_{rc}$ the dominant pole
 * @return
 */
inline double PCSSM::coPoleOneAngFreq() const
{
    return 2/(static_cast<double>(resload)*capout);
}
/**********************DCM************************/
/**
 * @brief coZeroTwoAngFreq - $\omega_{zrhp}$ - rhp zero
 * @return
 */
inline double PCSSM::coDCMZeroTwoAngFreq() const
{
    double tmp = std::pow(turnrat, 2)*static_cast<double>(resload);
    return tmp/(priminduct*voltrat*(voltrat+1));
}

/**
 * @brief coPoleTwoAngFreq - $\omega_{p2}$ - pole
 * @return
 */
inline double PCSSM::coDCMPoleTwoAngFreq() const
{
    return (std::pow(turnrat,2)*static_cast<double>(resload))/(priminduct*std::pow((voltrat+1),2));
}

/**
 * @brief coDCMCriticValue - $K_{vd}$ -
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
 * @brief coCurrDetectSlopeVolt - $S_{n}$ - the voltage slope when the primary-side current is detected on RS_s
 *        The inductor rising slope.
 * @param rsense - the value of current sense resistor
 * @return
 */
inline double PCSSM::coCurrDetectSlopeVolt(double rsense) const
{
    return (voltin*rsense)/priminduct;
}

/**
 * @brief coTimeConst - $\tau_{L}$ - switching period
 * @param fsw
 * @return
 */
inline double PCSSM::coTimeConst(float fsw) const
{
    return (2*priminduct*static_cast<double>(fsw))/(std::pow(turnrat,2)*static_cast<double>(resload));
}

/**
 * @brief coGainCurrModeContrModulator - $F_{m}$ - the PWM modulator gain
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
 * @brief coDutyToOutTrasfFunct - $G_{vd}(s)$ -
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
        result = -1; //it's very bad way, i will rewrite this after
    }
    return result;
}

/**
 * @brief coControlToOutTransfFunct - $G_{vc}(s)$ - The control-to-output transfer function
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
        result = -1;
    }
    return result;
}
/**********************CCM****************************/
/**
 * @brief coCCMZeroTwoAngFreq - $\omega_{zrhp}$ -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMZeroTwoAngFreq(double duty) const
{
    double tmp = std::pow(turnrat,2)*std::pow((1-duty),2)*static_cast<double>(resload);
    return tmp/(duty*priminduct);
}

/**
 * @brief coCCMPoleTwoAngFreq - $\omega_{o}$ -
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
 * @brief coDCMVoltGainCoeff - $K_{vd}$ -
 * @param duty
 * @return
 */
inline double PCSSM::coCCMVoltGainCoeff(double duty) const
{
    return static_cast<double>(voltin)/(static_cast<double>(turnrat)*(std::pow((1-duty),2)));
}

/**
 * @brief coDCMCurrGainCoeff - $K_{id}$ -
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
 * @brief coDCMQualityFact - $Q$ -
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
 * @brief coDCMDutyToInductCurrTrasfFunct - $G_{id}(s)$ -
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

/**************************FCCD*************************/
/**************************CCM**************************/
/**
 * @brief coOptoTransfFunct - $G_{opto}(s)$ -
 * @param s
 * @param fdrp
 * @return
 */
inline double FCCD::coOptoTransfFunct(double s, double fdrp) const
{
    double tmp = 1/(1+(s/(2*S_PI*S_OPTO_POLE)));
    return (refres/resoptdiode(fdrp))*tmp;
}

/**
 * @brief coContrToOutTransfFunct - $G_{co}(s)$ -
 * @param s
 * @param capout
 * @param esrcout
 * @param resload
 * @return
 */
inline double FCCD::coContrToOutTransfFunct(double s, double capout, double esrcout, int16_t resload) const
{
    return (1+s*(capout*resload))/(1+s*capout*(resload+esrcout));
}

/**
 * @brief coTransfFunct - $T_{s}(s)$ -
 * @param s
 * @param fdrp
 * @param capout
 * @param esrcout
 * @param resload
 * @return
 */
inline double FCCD::coTransfFunct(double s, double fdrp, double capout, double esrcout, int16_t resload) const
{
    return todB(coOptoTransfFunct(s, fdrp))+todB(coContrToOutTransfFunct(s, capout, esrcout, resload))+todB(coOptoTransfGain(fdrp));
}

/**
 * @brief coResCap2 -
 * @param s
 * @param fdrp
 * @param capout
 * @param esrcout
 * @param resload
 */
void FCCD::coResCap2(double s, double fdrp, double capout, double esrcout, int16_t resload)
{
    double index = std::pow(10, (-1*(todB(coOptoTransfFunct(s, fdrp))+todB(coContrToOutTransfFunct(s, capout, esrcout, resload))+todB(coOptoTransfGain(fdrp)))));
    rcap2 = static_cast<int16_t>(resup*index);
}

/**
 * @brief coCap1 -
 */
void FCCD::coCap1()
{
    if(rcap2 != NULL)
    {
        cap1 = (capout*capoutesr)/rcap2;
    }
    else
    {
        //sorry this bad way;
    }
}

/**
 * @brief coCap2 -
 */
void FCCD::coCap2()
{
    if(rcap2 != NULL)
    {
        cap1 = (capout*voltout)/(rcap2*static_cast<double>(curroutmax));
    }
    else
    {
        //sorry this bad way;
    }
}

/**
 * @brief coOptoTransfGain - $K_{c}$ -
 * @return
 */
inline double FCCD::coOptoTransfGain(double fdrp) const
{
    double kd = resdown/static_cast<double>((resup+resdown));
    return (optoctr*kd*refres)/resoptdiode(fdrp);
}

/**
 * @brief coTransfZero - $\omega_{z}$
 * @return
 */
inline double FCCD::coTransfZero() const
{
    return 1/(cap1*rcap2);
}

/**
 * @brief coTransfPoleOne - $\omega_{p1}$
 * @return
 */
inline double FCCD::coTransfPoleOne() const
{
    return 1/(refcap*refres);
}

/**
 * @brief coCCMTransfPoleZero - $\omega_{p0}$
 * @return
 */
inline double FCCD::coCCMTransfPoleZero() const
{
    return 1/((cap1+cap2)*rcap1);
}

/**
 * @brief coTransfPoleTwo - $\omega_{p2}$
 * @return
 */
inline double FCCD::coTransfPoleTwo() const
{
    return (cap1+cap2)/(cap1*cap2*rcap2);
}

/**
 * @brief coOptoFeedbTransfFunc -
 * @param s
 * @param mode
 * @return
 */
inline double FCCD::coOptoFeedbTransfFunc(double s, PS_MODE mode)
{
    double result = 0.0;
    double num = 1+(s/coTransfZero());
    double dnm = 0.0;
    if(mode == CCM_MODE)
    {
        dnm = (s/coCCMTransfPoleZero())*(1+(s/coTransfPoleOne()))*(1+(s/coTransfPoleTwo()));
        result = num/dnm;
    }
    else if(mode == DCM_MODE)
    {
        dnm = (s/coDCMTransfPoleZero())*(1+(s/coTransfPoleOne()));
        result = num/dnm;
    }
    else
    {
        result = -1;
    }
    return result;
}
/**************************DCM**************************/
/**
 * @brief coDCMTransfPoleZero - $\omega_{p2}$
 * @return
 */
inline double FCCD::coDCMTransfPoleZero() const
{
    return 1/(cap1*rcap1);
}
