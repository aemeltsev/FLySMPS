#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

//enum PS_MODE
//{
//    CCM_MODE,
//    DCM_MODE
//};

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
    inline double coCCMZeroTwoAngFreq() const;
    inline double coCCMPoleTwoAngFreq() const;
    inline double coCCMGainZero(double duty, float fsw, float vs = 2.0f) const;
    /**
     * @brief coGetExternAddVolt
     * @param se
     */
    void coSetExternAddVolt(double se){sawvolt = se;}
    inline double coCurrDetectSlopeVolt(double rsense) const;
    inline double coTimeConst(float fsw) const;
    inline double coGainCurrModeContrModulator(double rsense, float fsw) const;
    inline double coDutyToOutTrasfFunct(double s, double rsense, float fsw);
    inline double coControlToOutTransfFunct(double s, double rsense, float fsw);
    /************DCM***********/
    inline double coDCMZeroTwoAngFreq(double duty) const;
    inline double coDCMPoleTwoAngFreq(double duty) const;
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
