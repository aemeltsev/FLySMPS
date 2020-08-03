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
    inline double coZeroTwoAngFreq() const;
    inline double coZeroOneAngFreq() const;
    inline double coPoleTwoAngFreq() const;
    inline double coPoleOneAngFreq() const;
    inline double coGainZero(double duty, float fsw, float vs = 2.0f) const;
    /**
     * @brief coGetExternAddVolt
     * @param se
     */
    void coGetExternAddVolt(double se){sawvolt = se;}
    inline double coCurrDetectSlopeVolt(double rsense) const;
    inline double coTimeConst(float fsw) const;
    inline double coGainCurrModeContrModulator(double rsense, float fsw) const;
    inline double coDutyToOutTrasfFunct(double s, double rsense, float fsw);
    inline double coControlToOutTransfFunct(double s, double rsense, float fsw);

private:
    float turnrat;
    double priminduct;
    float resload;
    int16_t voltin;
    int16_t voltout;
    double capout;
    double esrcap;
    double voltrat = voltout/voltin;
    double sawvolt;

};

class FCCD
{
public:
    FCCD();
};

#endif // CONTROLOUT_H
