/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <QtMath>
#include <QVector>
#include <cstdint>

#define S_TL431_VREF           2.5      //V_TL431_min - the TL431 minimum operating voltage V
#define S_TL431_CURR_CATH      1.5*1E-3 //I_TL431_bias - the additional TL431 bias current A
#define S_OPTO_CE_SAT          2.5*1E-1 //V_CE_sat - optocoupler saturation voltage V
#define S_OPTO_FORVARD_DROP    1        //V_f - the LED forvard voltage A
#define S_INT_BIAS_CONTR       5        //V_ccp - the pull-up Vcc level on primary side V
#define S_OPTO_POLE            10000    //f_opto - the optocoupler pole that has ben characterized with R_pullup Hz
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

    double sawvolt; //the externally added voltage - S_e(The compensation slope)
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
     * @brief PCSSM - Power circuit small-signal model aka the control-to-output
     *                transfer function.
     *                For more reference:
     *                Kleebchampee W.-Modeling and control design of a current-mode controlled
     *                                flyback converter with optocoupler feecdback.
     *                Wang E.-AN017.Feedback Control Design of Off-line Flyback Converter.
     *                Panov Y. et all.-Small-Signal Analysis and Control Design of
     *                                 Isolated Power Supplies with Optocoupler Feedback.
     *                Basso C.P.-Switch-Mode Power Supplies Spice Simulations and Practical Designs.
     * @param ssmvar - Preliminary design values for small-signal estimate
     * @param mode - select operation mode. Default - discontinuous current mode
     */
    PCSSM(SSMPreDesign &ssmvar, PS_MODE mode = DCM_MODE)
    {
        qSwap(m_ssmvar, ssmvar);
        m_mode = mode;
    }
    inline double coZeroOneAngFreq() const; //f_{zc}
    inline double coPoleOneAngFreq() const; //f_{rc}
    /************DCM***********/
    inline double coDCMZeroTwoAngFreq() const; //f_{zrhp}
    inline double coDCMPoleTwoAngFreq() const; //f_{p2}
    inline double coDCMCriticValue() const; //K_{vd}
    /************CCM***********/
    inline double coCCMZeroTwoAngFreq() const; //f_{zrhp}
    inline double coCCMPoleTwoAngFreq() const; //f_{o}
    inline double coCCMVoltGainCoeff() const; //K_{vd}
    inline double coCCMCurrGainCoeff() const; //K_{id}
    inline double coCCMQualityFact() const; //Q
    double coMagCCMDutyToInductCurrTrasfFunct(int32_t freq); //G_{id}(s)
    double coPhsCCMDutyToInductCurrTrasfFunct(int32_t freq); //G_{id}(s)
    inline double coCurrDetectSlopeVolt() const; //S_{n}
    inline double coTimeConst() const; //\tau_{L}
    inline double coGainCurrModeContrModulator() const; //F_{m}
    double coMagDutyToOutTrasfFunct(int32_t freq); //G_{vd}(s)
    double coPhsDutyToOutTrasfFunct(int32_t freq); //G_{vd}(s)
    double coMagControlToOutTransfFunct(int32_t freq); //G_{vc}(s)
    double coPhsControlToOutTransfFunct(int32_t freq); //G_{vc}(s)
    void coGainOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_mag);
    void coPhaseOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_phase);
private:
    SSMPreDesign m_ssmvar;
    PS_MODE m_mode;
};

struct FCPreDesign
{
    int16_t out_voltage; //Output voltage for control
    float out_current;
    int32_t res_pull_up; //Reference resistor in pwm side controller
    int16_t res_down; //Down resistor in voltage divider K_{d}
    int16_t phase_shift; //Phase shift value for phase margine in controll loop(P degre)
    int16_t amp_gaim_marg; //Gain margine(M degre)
    int16_t opto_ctr; //The minimum current transfer ratio in optocoupler
    int32_t freq_sw;
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

struct LCSecondStage
{
    double lcf_ind;
    double lcf_cap;
    float lcf_cap_esr;
};

class FCCD
{
public:
    /**
     * @brief FCCD - Feedback compensation circuit design
     *        For more reference:
     *        Basso C.-Designing control loops for linear and switch mode power supples.
     *        Kleebchampee W.-Modeling and control design of a current-mode controlled
     *                        flyback converter with optocoupler feecdback.
     *        Hua L.-Design Considerations and Small Signal Modeling of the Flyback Converter
     *               Using Second Stage LC Filtering Circuit.
     *        Srinivasa Rao M.-Designing flyback converters using peak-current-mode controllers.
     */
    FCCD(FCPreDesign &fcvar, RampSlopePreDesign &rsvar, LCSecondStage &lcfvar, PS_MODE mode = DCM_MODE)
    {
        qSwap(m_fcvar, fcvar);
        qSwap(m_rsvar, rsvar);
        qSwap(m_lcfvar, lcfvar);
        m_mode = mode;
    }
    //1.
    inline int32_t coResOptoDiode() const; //R_{LED_max}
    inline int32_t coResOptoBias() const; //R_{bias}
    inline int32_t coResUp() const; //R_{1} or R_{up} in voltage divider K_{d}
    inline double coVoltageDivideGain() const; //K_{d}
    inline double coVoltageOptoGain() const; //K_{c} or G_{1} in Db 20*log10(K_{c})
    //2.
    inline double coExterRampSlope() const; //S_{e}
    inline double coIndOnTimeSlope() const; //S_{n}
    inline double coCompRamp() const; //M_{c}
    inline double coQuality() const; //Q_{p}
    //3.
    double coFreqCrossSection() const; //f_{cross}
    //4.
    inline double coFreqPole() const; //f_{pole}
    inline double coFreqZero() const; //f_{zero}
    //5.
    int32_t coResZero() const; //R_{f} or R_{zero}
    inline double coCapZero() const; //C_{f} or C_{zero}
    inline double coCapPoleOpto() const; //C_{opto} - pulldown capacitor in hight optocoupler side
    //6.
    inline double coMagLCTransfFunc(int32_t freq) const; //H_{lc}(s)
    inline double coMagOptoFeedbTransfFunc(int32_t freq) const; //H(s)
    inline double coPhsLCTransfFunc(int32_t freq) const;
    inline double coPhsOptoFeedbTransfFunc(int32_t freq) const;
    //7.
    void coGainOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_mag);
    void coPhaseOptoFeedbTransfFunc(QVector<int32_t> &in_freq, QVector<double> &out_phase);

private:
    FCPreDesign m_fcvar;
    RampSlopePreDesign m_rsvar;
    LCSecondStage m_lcfvar;
    PS_MODE m_mode;

    inline float coInvDutyCycle(){return 1 - m_rsvar.actual_duty;} //D^{`}
    inline int16_t coBoost() const {return m_fcvar.amp_gaim_marg - m_fcvar.phase_shift - 90;} //Boost
    inline double coTransfZero() const; //f_{z}
    inline double coTransfPoleOne() const; //f_{p1}
    inline double coTransfLCZero() const; //f_{lc-z}
    inline double coQualityLC() const; //Q_{lc}
};

#endif // CONTROLOUT_H
