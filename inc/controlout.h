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
#include <QObject>
#include <QtMath>
#include <QVector>
#include <cstdint>

#define S_TL431_VREF           2.5      //V_TL431_min - the TL431 minimum operating voltage V
#define S_TL431_CURR_CATH      0.0015   //I_TL431_bias - the additional TL431 bias current A
#define S_OPTO_CE_SAT          0.25     //V_CE_sat - optocoupler saturation voltage V
#define S_OPTO_FORVARD_DROP    1        //V_f - the LED forvard voltage A
#define S_INT_BIAS_CONTR       5        //V_ccp - the pull-up Vcc level on primary side V
#define S_OPTO_POLE            10000    //f_opto - the optocoupler pole that has ben characterized with R_pullup Hz
#define M_PI_DEG               180

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

class PCSSM: public QObject
{
    Q_OBJECT
signals:
    void arrayMagSSMComplete();
    void arrayPhaseSSMComplete();

private:
    SSMPreDesign m_ssmvar;
    PS_MODE m_mode;

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
    PCSSM(SSMPreDesign &ssmvar, PS_MODE mode = DCM_MODE);

    /********************COM*************************/

    /**
     * @brief coZeroOneAngFreq - $\f_{zc}$ - esr zero, lhp
     * @return esr zero value, angular frequency
     */
    double coZeroOneAngFreq() const;

    /**
     * @brief coPoleOneAngFreq - $\f_{rc}$ the dominant pole
     * @return
     */
    double coPoleOneAngFreq() const;

    /********************COM*************************/
    /********************DCM*************************/

    /**
     * @brief coZeroTwoAngFreq - $\f_{zrhp}$ - rhp zero
     * @return
     */
    double coDCMZeroTwoAngFreq() const;

    /**
     * @brief coPoleTwoAngFreq - $\f_{p2}$ - pole
     * @return
     */
    double coDCMPoleTwoAngFreq() const;

    /**
     * @brief coDCMCriticValue - $K_{vd}$ -
     * @return
     */
    double coDCMCriticValue() const;

    /********************DCM*************************/
    /********************CCM*************************/

    /**
     * @brief coCCMZeroTwoAngFreq - $f_{zrhp}$
     * @return
     */
    double coCCMZeroTwoAngFreq() const;

    /**
     * @brief coCCMPoleTwoAngFreq - $f_{o}$
     * @return
     */
    double coCCMPoleTwoAngFreq() const;

    /**
     * @brief coDCMVoltGainCoeff - $K_{vd}$
     * @return
     */
    double coCCMVoltGainCoeff() const;

    /**
     * @brief coDCMCurrGainCoeff - $K_{id}$
     * @return
     */
    double coCCMCurrGainCoeff() const;

    /**
     * @brief coDCMQualityFact - $Q$
     * @return
     */
    double coCCMQualityFact() const;

    /**
     * @brief coDCMDutyToInductCurrTrasfFunct - $G_{id}(s) magnitude value for current frequency$
     * @param freq
     * @return
     */
    double coMagCCMDutyToInductCurrTrasfFunct(const double freq);

    /**
     * @brief coPhsCCMDutyToInductCurrTrasfFunct - $G_{id}(s) phase value of current frequency$
     * @param freq
     * @return
     */
    double coPhsCCMDutyToInductCurrTrasfFunct(const double freq);

    /********************CCM*************************/
    /********************F_m*************************/

    /**
     * @brief coCurrDetectSlopeVolt - $S_{n}$ - the voltage slope when the primary-side current is detected on RS_s
     *        The inductor rising slope.
     * @return
     */
    double coCurrDetectSlopeVolt() const;

    /**
     * @brief coTimeConst - $\tau_{L}$ - switching period
     * @return
     */
    inline double coTimeConst() const;

    /**
     * @brief coGainCurrModeContrModulator - $F_{m}$ - the PWM modulator gain
     * @return
     */
    inline double coGainCurrModeContrModulator() const;

    /********************F_m*************************/
    /********************OUT*************************/

    /**
     * @brief coMagDutyToOutTrasfFunct - $G_{vd}(s)$ -
     * @param freq
     * @return
     */
    double coMagDutyToOutTrasfFunct(const double freq);

    /**
     * @brief coPhsDutyToOutTrasfFunct - $G_{vd}(s)$ -
     * @param freq
     * @return
     */
    double coPhsDutyToOutTrasfFunct(const double freq);

    /**
     * @brief coMagControlToOutTransfFunct - $G_{vc}(s)$ - The control-to-output transfer function
     * @param freq
     * @return
     */
    double coMagControlToOutTransfFunct(const double freq);

    double coPhsControlToOutTransfFunct(const double freq);

    void coGainControlToOutTransfFunct(QVector<double> &in_freq, QVector<double> &out_mag);

    void coPhaseControlToOutTransfFunct(QVector<double> &in_freq, QVector<double> &out_phase);

    /********************OUT*************************/
};

struct FCPreDesign
{
    int16_t out_voltage; //Output voltage for control
    double out_current;
    double res_pull_up; //Reference resistor in pwm side controller
    double res_down; //Down resistor in voltage divider K_{d}
    int32_t phase_rotate; //Phase shift value for phase margine in controll loop(/_H(f_c))
    int32_t phase_marg; //Phase margine(\phi_m)
    double opto_ctr; //The minimum current transfer ratio in optocoupler
    int32_t freq_sw;
    double opto_inner_cap; //Parasitic capacitance value in optocoupler transistor
    double out_sm_cap; //Smooting capacitor after diode
    double out_sm_cap_esr; //Sequence resistance insecondary smooting capacitor
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
    double lcf_cap_esr;
};

class FCCD: public QObject
{
    Q_OBJECT
private:
    FCPreDesign m_fcvar;
    RampSlopePreDesign m_rsvar;
    LCSecondStage m_lcfvar;
    PS_MODE m_mode;

    /**
     * @brief coBoost - Boost
     * @return
     */
    inline int16_t coBoost() const
    {
        return m_fcvar.phase_marg - m_fcvar.phase_rotate - 90;
    }

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
    FCCD(FCPreDesign &fcvar, RampSlopePreDesign &rsvar, LCSecondStage &lcfvar, PS_MODE mode = DCM_MODE);

    /**
     * @brief coFreqCrossSection - f_{cross}
     * @return
     */
    double coFreqCrossSection() const;

    /**
     * @brief coFreqPole - f_{pole}
     * @return
     */
    double coFreqPole() const;

    /**
     * @brief coFreqZero - f_{zero}
     * @return
     */
    double coFreqZero() const;

    //1.
    /**
     * @brief coResOptoDiode - R_{LED_max}
     * @return
     */
    double coResOptoDiode() const;

    //2.
    /**
     * @brief coResUp - R_{1} or R_{up} in voltage divider K_{d}
     * @return
     */
    double coResUp() const;

    //3.
    /**
     * @brief coResOptoBias - R_{bias}
     * @return
     */
    double coResOptoBias() const;

    //4.
    /**
     * @brief coVoltageOptoGain - K_{c} or G_{0} in Db 20*log10(K_{c})
     * @return
     */
    double coVoltageOptoGain() const;

    //5.
    /**
     * @brief coResZero - R_{f} or R_{zero}
     * @return
     */
    double coResZero() const;

    //6.
    /**
     * @brief coCapZero - C_{f} or C_{zero}
     * @return
     */
    double coCapZero() const;

    //7.
    /**
     * @brief coCapPoleOpto - C_{opto} - pulldown capacitor in hight optocoupler side
     * @return
     */
    double coCapPoleOpto() const;

    //8.
    double coResDivideGain() const;

    //9.
    /**
     * @brief coTransfZero - $\f_{z}$
     * @return
     */
    double coTransfZero() const;

    /**
     * @brief coTransfPoleOne - $\f_{p1}$
     * @return
     */
    double coTransfPoleOne() const;

    //10.
    /**
     * @brief coTranfRCZero - \omega_{zesr}
     * @return
     */
    double coTranfRCZero() const;

    /**
     * @brief coTransfLCZero - \omega_{op}
     * @return
     */
    double coTransfLCZero() const;

    /**
     * @brief coQualityLC - Q_{lc}
     * @return
     */
    double coQualityLC() const;

    /////////////////////////////////////////////
    /**
     * @brief coIndOnTimeSlope - S_{n}
     * @return
     */
    double coIndOnTimeSlope() const;

    /**
     * @brief FCCD::coCompRamp - M_{c}
     * @return
     */
    double coCompRamp() const;

    /**
     * @brief FCCD::coExterRampSlope - S_{e}
     * @return
     */
    double coExterRampSlope() const;

    /**
     * @brief coQuality - Q_{p}
     * @return
     */
    double coQuality() const;

    /////////////////////////////////////////////
    /**
     * @brief coLCTransfFunc - |H_{lc}(s)|
     * @param s
     * @return
     */
    double coMagLCTransfFunc(const double freq) const;

    /**
     * @brief coOptoFeedbTransfFunc - |H(s)|
     * @param s
     * @return
     */
    double coMagOptoFeedbTransfFunc(const double freq) const;

    /**
     * @brief coPhsLCTransfFunc - /_ H_{lc}(s)
     * @param freq
     * @return
     */
    double coPhsLCTransfFunc(const double freq) const;

    /**
     * @brief coPhsOptoFeedbTransfFunc - /_ H(s)
     * @param freq
     * @return
     */
    double coPhsOptoFeedbTransfFunc(const double freq) const;

    //7.
    /**
     * @brief coGainOptoFeedbTransfFunc - create the 20*log_10(|H(s)|) sequence values
     * @param freq
     * @return
     */
    void coGainOptoFeedbTransfFunc(QVector<double> &in_freq, QVector<double> &out_mag);

    /**
     * @brief coPhaseOptoFeedbTransfFunc
     * @param freq
     * @return
     */
    void coPhaseOptoFeedbTransfFunc(QVector<double> &in_freq, QVector<double> &out_phase);
};
#endif // CONTROLOUT_H
