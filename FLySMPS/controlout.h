#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <QtMath>
#include <QVector>
#include <cmath>
#include <cstdint>

#define S_TL431_VREF           2.5      //V
#define S_TL431_CURR_CATH      1.5*1E-3 //bias current A
#define S_OPTO_CE_SAT          2.5*1E-1 //saturation voltage V
#define S_OPTO_FORVARD_DROP    1        //V
#define S_INT_BIAS_CONTR       5        //V
#define S_OPTO_POLE  1000     //Hz
enum PS_MODE
{
    CCM_MODE = 0,
    DCM_MODE = 1
};

struct SSMPreDesign
{
    int16_t input_voltage; //Input voltage
    int32_t freq_switch; //Frequency of the power switch
    float actual_duty; //Actual duty cycle
    double primary_ind; //Primary inductance
    double res_sense; // Current sense resitor

    //Secondary side output for control
    int16_t output_voltage; //Output voltage
    float output_full_load_res; //Load resistance
    float turn_ratio; //Turns ratio
    double output_cap; //Output capacitance
    double output_cap_esr; //Parasitic ESR of output capacitor
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
     * @param ssmvar - Preliminary design values for small-signal estimate
     */
    PCSSM(SSMPreDesign &ssmvar, PS_MODE mode = DCM_MODE)
    {
        qSwap(m_ssmvar, ssmvar);
        m_mode = mode;
    }
    inline double coZeroOneAngFreq() const; //\omega_{zc}
    inline double coPoleOneAngFreq() const; //\omega_{rc}
    /************DCM***********/
    inline double coDCMZeroTwoAngFreq() const; //\omega_{zrhp}
    inline double coDCMPoleTwoAngFreq() const; //\omega_{p2}
    inline double coDCMCriticValue() const; //K_{vd}
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
    inline double coDutyToOutTrasfFunct(double s); //G_{vd}(s)
    inline double coControlToOutTransfFunct(double s); //G_{vc}(s)
private:
    SSMPreDesign m_ssmvar;
    PS_MODE m_mode;
    double sawvolt; //S_e
};

struct FCPreDesign
{
    int16_t out_voltage; //Output voltage for control
    int32_t res_pull_up; //Reference resistor in pwm side controller
    int16_t res_down; //Down resistor in voltage divider K_{d}
    int16_t phase_shift; //Phase shift value for phase margine in controll loop
    int16_t amp_gaim_marg; //Gain margine
    int16_t opto_ctr; //The current transfer ratio in optocoupler
    double opto_inner_cap; //Parasitic capacitance value in optocoupler transistor

};

struct RampSlopePreDesign
{
    int16_t inp_voltage;
    int16_t prim_turns;
    int16_t sec_turns_to_control;
    float actual_duty; //Actual duty cycle
    float out_pwr_tot;
    double primary_ind;
    double res_sense; // Current sense resitor
};

class FCCD
{
public:
    /**
     * @brief FCCD - Feedback compensation circuit design
     */
    FCCD(int16_t vout, int16_t rpup,
         int16_t ctr, int16_t rdwn,
         int16_t pshft, int16_t gain,
         double copto)
        :volt_to_cont(vout), res_pull_up(rpup)
        ,opto_ctr(ctr), res_down(rdwn)
        ,phase_shift_val(pshft), ampl_gain_val(gain)
        ,opto_inner_cap(copto)
    {}

    inline int16_t coResOptoDiode() const;
    inline int16_t coResOptoBias() const;
    inline int16_t coResUp() const;
    inline double coVoltageDivideGain() const; //K_{d}
    inline double coVoltageOptoGain() const; //K_{c}
    void coSetRampSlopeVal(int16_t vin, double rcursns,
                           double indprim, int16_t duty,
                           int16_t numprim, int16_t numsec,
                           double ptot)
    {
        volt_inp=vin;
        res_cur_sense=rcursns;
        induct_prim=indprim;
        duty_cycle=duty;
        number_prim_turns=numprim;
        number_sec_turns=numsec;
        power_out_tot=ptot;
    }
    inline double coExterRampSlope() const; //S_{e}
    inline double coIndOnTimeSlope() const; //S_{n}
    inline double coCompRamp() const; //M_{c}
    inline double coQuality() const; //Q_{p}
    inline double coFreqCrossSection() const; //f_{cross}
    inline double coKFactor() const; //k
    inline double coFreqPole() const; //f_{pole}
    inline double coFreqZero() const; //f_{zero}
    inline double coCapPoleOpto() const;
    inline double coCapOpto() const; //C_{opto}
    inline double coResErrorAmp(double control_to_out) const; //R_{f}
    inline double coCapErroAmp(int16_t frq_ea_zero, int16_t res_ea) const; //C_{f}
    inline double coOptoFeedbTransfFunc(int16_t freq, double control_to_out, int16_t frq_ea_zero, int16_t res_ea);
    inline double coGainOptoFeedbTransfFunc(int16_t freq, double control_to_out, int16_t frq_ea_zero, int16_t res_ea);
    inline double coPhaseOptoFeedbTransfFunc(int16_t freq, double control_to_out, int16_t frq_ea_zero, int16_t res_ea);

private:
    int16_t volt_to_cont;
    int16_t res_pull_up;
    int16_t opto_ctr;
    int16_t res_down;
    int16_t phase_shift_val; //P degre
    int16_t ampl_gain_val; //M degre
    double opto_inner_cap;

    int16_t volt_inp=0;
    double res_cur_sense=0;
    double induct_prim=0;
    int16_t duty_cycle=0;
    int16_t number_prim_turns=0;
    int16_t number_sec_turns=0;
    double power_out_tot=0;

    int16_t inv_duty_cycle=1-duty_cycle;
    inline int16_t coBoost() const {return ampl_gain_val-phase_shift_val-90;}
    inline double coGainErrorAmp(double control_to_out) const;
    inline double coOptoTransfGain() const; //K_{c}
    inline double coTransfZero(double control_to_out, int16_t frq_ea_zero, int16_t res_ea) const; //\omega_{z}
    inline double coTransfPoleOne() const; //\omega_{p1}
};

#endif // CONTROLOUT_H
