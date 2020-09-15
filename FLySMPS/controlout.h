#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

#define S_VREF       2.5      //V
#define S_CURR_CATH  1.5*1E-3 //A
enum PS_MODE
{
    CCM_MODE,
    DCM_MODE
};

class PCSSM
{
public:
    /**
     * @brief PCSSM - Power circuit small-signal sodel
     * @param tr - Turns ratio
     * @param lp - Primary inductance
     * @param lres - Load resistance
     * @param vin - Input voltage
     * @param vout - Output voltage
     * @param cout - Output capacitance
     * @param esr - Parasitic ESR of output capacitor
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
        voltrat = voltout/voltin;
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
     * @brief coGetExternAddVolt - The compensation slope.
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
    double voltrat;
    double sawvolt; //S_e
};

class FCCD
{
public:
    /**
     * @brief FCCD - Feedback compensation circuit design
     * @param ctr - The current transfer ratio
     * @param vout - Output voltage
     * @param rdwn - Down resistor in voltage divider K_{d}
     * @param rrf
     * @param crf
     * @param rc1
     * @param c1
     * @param rc2
     * @param c2
     */

    FCCD(double ctr, int16_t vout,
         int16_t rrf, int16_t rdwn,
         int16_t rc1, double c1,
         int16_t rc2, double c2):
        optoctr(ctr), voltout(vout),
        resdown(rdwn), refres(rrf),
        rcap1(rc1), cap1(c1),
        rcap2(rc2), cap2(c2)
    {
        resup = rdwn * static_cast<int16_t>((vout/S_VREF)) - 1;
    }

    inline double coOptoTransfGain(double fdrp) const; //K_c
    inline double coTransfZero() const; //omega_z
    inline double coTransfPoleOne() const; //omega_p1
    inline double coCCMTransfPoleZero() const; //omega_p0
    inline double coDCMTransfPoleZero() const; //omega_p0
    inline double coTransfPoleTwo() const; //omega_p2
    inline double coOptoFeedbTransfFunc(double s, PS_MODE mode); //G_c(s)

private:
    double optoctr;
    int16_t voltout;
    int16_t resdown;
    int16_t refres;

    int16_t resup;
    inline double resoptdiode(double fdrp) const {return (voltout-S_VREF-fdrp)/S_CURR_CATH;}
    double refcap;
    int16_t rcap1;
    double cap1;
    int16_t rcap2;
    double cap2;
};

#endif // CONTROLOUT_H
