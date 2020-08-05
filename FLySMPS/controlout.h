#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

enum PS_MODE
{
    CCM_MODE,
    DCM_MODE
};

class PCSSM
{
public:
    /**
     * @brief PCSSM
     * @param tr
     * @param lp
     * @param lres
     * @param vin
     * @param vout
     * @param cout
     * @param esr
     */
    PCSSM(float tr, double lp,
          float lres, int16_t vin,
          int16_t vout, double cout,
          double esr):
        turnrat(tr), priminduct(lp),
        resload(lres), voltin(vin),
        voltout(vout), capout(cout),
        esrcap(esr)
    {}
    inline double coZeroOneAngFreq() const;
    inline double coPoleOneAngFreq() const;
    /************CCM***********/
    inline double coDCMZeroTwoAngFreq() const;
    inline double coDCMPoleTwoAngFreq() const;
    inline double coDCMGainZero(double duty, float fsw, float vs = 2.0f) const;
    /**
     * @brief coGetExternAddVolt
     * @param se
     */
    void coSetExternAddVolt(double se){sawvolt = se;}
    inline double coCurrDetectSlopeVolt(double rsense) const;
    inline double coTimeConst(float fsw) const;
    inline double coGainCurrModeContrModulator(double rsense, float fsw) const;
    /************DCM***********/
    inline double coCCMZeroTwoAngFreq(double duty) const;
    inline double coCCMPoleTwoAngFreq(double duty) const;
    inline double coCCMVoltGainCoeff(double duty) const;
    inline double coCCMCurrGainCoeff(double duty) const;
    inline double coCCMQualityFact(double duty) const;
    inline double coCCMDutyToInductCurrTrasfFunct(double s, double duty);

    inline double coDutyToOutTrasfFunct(double s, double rsense, float fsw, double duty, PS_MODE mode);
    inline double coControlToOutTransfFunct(double s, double rsense, float fsw, double duty, PS_MODE mode);
private:
    float turnrat;
    double priminduct;
    float resload;
    int16_t voltin;
    int16_t voltout;
    double capout;
    double esrcap;
    double voltrat = voltout/voltin;
    double sawvolt; //S_e
};

class FCCD
{
public:
    FCCD();

};

#endif // CONTROLOUT_H
