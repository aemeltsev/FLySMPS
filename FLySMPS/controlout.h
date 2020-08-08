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
    {

    }
    inline double coZeroOneAngFreq() const;
    inline double coPoleOneAngFreq() const;
    /************CCM***********/
    inline double coDCMZeroTwoAngFreq() const;
    inline double coDCMPoleTwoAngFreq() const;
    inline double coDCMCriticValue(float fsw) const;
    /************DCM***********/
    inline double coCCMZeroTwoAngFreq(double duty) const;
    inline double coCCMPoleTwoAngFreq(double duty) const;
    inline double coCCMVoltGainCoeff(double duty) const;
    inline double coCCMCurrGainCoeff(double duty) const;
    inline double coCCMQualityFact(double duty) const;
    inline double coCCMDutyToInductCurrTrasfFunct(double s, double duty);

    /**
     * @brief coGetExternAddVolt
     * @param se
     */
    void coSetExternAddVolt(double se){sawvolt = se;}
    inline double coCurrDetectSlopeVolt(double rsense) const;
    inline double coTimeConst(float fsw) const;
    inline double coGainCurrModeContrModulator(double rsense, float fsw) const;
    inline double coDutyToOutTrasfFunct(double s, float fsw, double duty, PS_MODE mode);
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
    /**
     * @brief FCCD
     * @param ctr
     * @param rup
     * @param rdwn
     * @param rdiode
     * @param rrf
     * @param crf
     * @param rc1
     * @param c1
     * @param rc2
     * @param c2
     */
    FCCD(double ctr, int16_t rup,
         int16_t rdwn, int16_t rdiode,
         int16_t rrf, double crf,
         int16_t rc1, double c1,
         int16_t rc2, double c2):
        optoctr(ctr), resup(rup),
        resdown(rdwn), resoptdiode(rdiode),
        refres(rrf), refcap(crf),
        rcap1(rc1), cap1(c1),
        rcap2(rc2), cap2(c2)
    {

    }

    inline double coOptoTransfGain() const; //K_c
    inline double coTransfZero() const; //omega_z
    inline double coTransfPoleOne() const; //omega_p1
    inline double coCCMTransfPoleZero() const; //omega_p0
    inline double coDCMTransfPoleZero() const; //omega_p0
    inline double coTransfPoleTwo() const; //omega_p2
    inline double coOptoFeedbTransfFunc(double s, PS_MODE mode); //G_c(s)

private:
    double optoctr;
    int16_t resup;
    int16_t resdown;
    int16_t resoptdiode;
    int16_t refres;
    double refcap;
    int16_t rcap1;
    double cap1;
    int16_t rcap2;
    double cap2;
};

#endif // CONTROLOUT_H
