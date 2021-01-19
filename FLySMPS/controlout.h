#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <QVector>
#include <cmath>
#include <cstdint>

#define S_VREF       2.5      //V
#define S_CURR_CATH  1.5*1E-3 //A
#define S_OPTO_POLE  1000     //Hz
enum PS_MODE
{
    CCM_MODE,
    DCM_MODE
};

class PCSSM
{
public:
    /**
     *                               ccm
     *
     *                (\omega_{rc})(\omega_{o})----
     *                                   |          \
     *                                   Q-----------> G_{id}(s)----------
     *                                              /                      \
     *                                   K_{id}-----                        \
     *                                                                       \
     * (\omega_{zc})(\omega_{zrhp})(\omega_{o})----                           \
     *                                              \                          \
     *                                   Q-----------> G_{vd}(s)----------------> G_{vc}(s)
     *                                              /                          /
     *                                   K_{vd}----                           /
     *                                                                       /
     *                                                  F_{m}---------------
     *                                                                     /
     *                                                  R_{s}-------------
     *
     *                               dcm
     *
     *                ---->(\omega_{zrhp})------\
     *              |                            \
     *              |                             \
     *        M ---- ---->(\omega_{p2})----------- \
     *                                              \
     *                     (\omega_{zc})-------------> G_{vd}(s)----------------> G_{vc}(s)
     *                                              /                          /
     *                     (\omega_{rc})-----------/                          /
     *                                            /                          /
     *                                           /                          /
     *        K-----------> K_{vd}--------------/                          /
     *                                                                    /
     *                                                 F_{m}-------------
     *
     * @brief PCSSM - Power circuit small-signal model
     * @param tr - Turns ratio
     * @param lp - Primary inductance
     * @param lres - Load resistance
     * @param vin - Input voltage
     * @param vout - Output voltage
     * @param cout - Output capacitance
     * @param esr - Parasitic ESR of output capacitor
     */
    PCSSM(int16_t vin, int16_t vout,
          float tr, float lres,
          double lp, double cout,
          double esr, double dt,
          double rsn, float frqsw):
        priminduct(lp), capout(cout),
        esrcap(esr), duty(dt),
        rsense(rsn), fsw(frqsw),
        resload(lres),turnrat(tr),
        voltin(vin),voltout(vout)
    {
        voltrat = voltout/voltin;
    }
    inline double coZeroOneAngFreq() const; //\omega_{zc}
    inline double coPoleOneAngFreq() const; //\omega_{rc}
    /************DCM***********/
    inline double coDCMZeroTwoAngFreq() const; //\omega_{zrhp}
    inline double coDCMPoleTwoAngFreq() const; //\omega_{p2}
    inline double coDCMCriticValue(float fsw) const; //K_{vd}
    /************CCM***********/
    inline double coCCMZeroTwoAngFreq() const; //\omega_{zrhp}
    inline double coCCMPoleTwoAngFreq() const; //\omega_{o}
    inline double coCCMVoltGainCoeff() const; //K_{vd}
    inline double coCCMCurrGainCoeff() const; //K_{id}
    inline double coCCMQualityFact() const; //Q
    inline double coCCMDutyToInductCurrTrasfFunct(double s); //G_{id}(s)

    /**
     * @brief coGetExternAddVolt - The compensation slope.
     * @param se
     */
    void coSetExternAddVolt(double se){sawvolt = se;}
    inline double coCurrDetectSlopeVolt() const; //S_{n}
    inline double coTimeConst() const; //\tau_{L}
    inline double coGainCurrModeContrModulator() const; //F_{m}
    inline double coDutyToOutTrasfFunct(double s, PS_MODE mode); //G_{vd}(s)
    inline double coControlToOutTransfFunct(double s, PS_MODE mode); //G_{vc}(s)
    void coPlotArray(QVector<double>& tabvector, PS_MODE mode, int16_t begin, int16_t end, int16_t step);
private:
    double priminduct;
    double capout;
    double esrcap;
    double voltrat;
    double sawvolt; //S_e
    double duty;
    double rsense;
    float fsw;
    float resload;
    float turnrat;
    int16_t voltin;
    int16_t voltout;
};

class FCCD
{
public:
    /**
     * @brief FCCD - Feedback compensation circuit design
     * @param vout - Output voltage
     * @param rrf - Reference resistor in pwm side controller
     * @param rdwn - Down resistor in voltage divider K_{d}
     * @param ctr - The current transfer ratio
     * @param cout - Output capacitor
     * @param coutesr - Output capacitor ESR
     * @param outcurr - Output current
     */

    FCCD(int16_t vout, int16_t rrf,
         int16_t rdwn, double ctr,
         double cout, double coutesr, float outcurr
         ):
        capout(cout), capoutesr(coutesr),
        optoctr(ctr), curroutmax(outcurr),
        voltout(vout), resdown(rdwn),
        refres(rrf)
    {
        resup = rdwn * static_cast<int16_t>((vout/S_VREF)) - 1;
    }

    inline double coOptoTransfFunct(double s, double fdrp) const; //G_opto(s)
    inline double coContrToOutTransfFunct(double s, double capout, double esrcout, int16_t resload) const; //G_co(s)
    inline double coTransfFunct(double s, double fdrp, double capout, double esrcout, int16_t resload) const; //T_{s}(s)

    void coResCap2(double s, double fdrp, double capout, double esrcout, int16_t resload);
    double coCap1();
    double coCap2();

    inline double coOptoTransfGain(double fdrp) const; //K_c
    inline double coTransfZero() const; //omega_z
    inline double coTransfPoleOne() const; //omega_p1
    inline double coCCMTransfPoleZero() const; //omega_p0
    inline double coDCMTransfPoleZero() const; //omega_p0
    inline double coTransfPoleTwo() const; //omega_p2
    inline double coOptoFeedbTransfFunc(double s, PS_MODE mode); //G_c(s)

private:
    double capout;
    double capoutesr;
    double optoctr;
    double refcap;
    float curroutmax;
    int16_t voltout;
    int16_t resdown; //resistor value in the divider, set in the constructor(down resistor)
    int16_t refres;
    int16_t resup; //resistor value in the output voltage divider(up resistor)

    //int16_t res_cap1;
    int16_t res_cap2;
    inline double todB(double input) const {return 20*std::log10(input);}
    inline double res_optic_diode(double fdrp) const {return (voltout-S_VREF-fdrp)/S_CURR_CATH;}
};

#endif // CONTROLOUT_H
