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

class PCSSM
{
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
    PCSSM(SSMPreDesign &ssmvar, PS_MODE mode = DCM_MODE)
    {
        qSwap(m_ssmvar, ssmvar);
        m_mode = mode;
    }

    /********************COM*************************/

    /**
     * @brief coZeroOneAngFreq - $\f_{zc}$ - esr zero, lhp
     * @return esr zero value, angular frequency
     */
    inline double coZeroOneAngFreq() const
    {
        return 1/(m_ssmvar.output_cap * m_ssmvar.output_cap_esr);
    }

    /**
     * @brief coPoleOneAngFreq - $\f_{rc}$ the dominant pole
     * @return
     */
    inline double coPoleOneAngFreq() const
    {
        return 2/(static_cast<double>(m_ssmvar.output_full_load_res) * m_ssmvar.output_cap);
    }

    /********************COM*************************/
    /********************DCM*************************/

    /**
     * @brief coZeroTwoAngFreq - $\f_{zrhp}$ - rhp zero
     * @return
     */
    inline double coDCMZeroTwoAngFreq() const
    {
        double voltrat = static_cast<double>(m_ssmvar.output_voltage)/m_ssmvar.input_voltage;
        double tmp = qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res);

        return tmp/(m_ssmvar.primary_ind * voltrat*(voltrat+1));
    }

    /**
     * @brief coPoleTwoAngFreq - $\f_{p2}$ - pole
     * @return
     */
    inline double coDCMPoleTwoAngFreq() const
    {
        double voltrat = static_cast<double>(m_ssmvar.output_voltage)/m_ssmvar.input_voltage;

        return (qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res))
                /(m_ssmvar.primary_ind * qPow((voltrat+1),2));
    }

    /**
     * @brief coDCMCriticValue - $K_{vd}$ -
     * @return
     */
    inline double coDCMCriticValue() const
    {
        double ktmp = (2. * m_ssmvar.primary_ind * static_cast<double>(m_ssmvar.freq_switch))
                /static_cast<double>(m_ssmvar.output_full_load_res);

        return static_cast<double>(m_ssmvar.input_voltage)
                /(static_cast<double>(m_ssmvar.turn_ratio) * qSqrt(ktmp));
    }

    /********************DCM*************************/
    /********************CCM*************************/

    /**
     * @brief coCCMZeroTwoAngFreq - $f_{zrhp}$
     * @return
     */
    inline double coCCMZeroTwoAngFreq() const
    {
        double tmp = qPow(m_ssmvar.turn_ratio, 2) * qPow((1-m_ssmvar.actual_duty), 2)
                * static_cast<double>(m_ssmvar.output_full_load_res);

        return tmp/(2 * M_PI * m_ssmvar.actual_duty * m_ssmvar.primary_ind);
    }

    /**
     * @brief coCCMPoleTwoAngFreq - $f_{o}$
     * @return
     */
    inline double coCCMPoleTwoAngFreq() const
    {
        double tmp = static_cast<double>(m_ssmvar.turn_ratio)
                /(2 * M_PI * qSqrt(m_ssmvar.output_cap * m_ssmvar.primary_ind));

        double num = qPow((1-m_ssmvar.actual_duty), 2);

        double ld = qSqrt((num*static_cast<double>(m_ssmvar.output_full_load_res))
                          /(static_cast<double>(m_ssmvar.output_full_load_res)+m_ssmvar.output_cap_esr));
        return tmp*ld;
    }

    /**
     * @brief coDCMVoltGainCoeff - $K_{vd}$
     * @return
     */
    inline double coCCMVoltGainCoeff() const
    {
        return static_cast<double>(m_ssmvar.input_voltage)
                /(static_cast<double>(m_ssmvar.turn_ratio) * (qPow((1-m_ssmvar.actual_duty),2)));
    }

    /**
     * @brief coDCMCurrGainCoeff - $K_{id}$
     * @return
     */
    inline double coCCMCurrGainCoeff() const
    {
        double tmp = 1+(2*m_ssmvar.actual_duty/(1-m_ssmvar.actual_duty));

        double dnm = qPow((1-m_ssmvar.actual_duty), 2);

        double mult = m_ssmvar.input_voltage
                /(static_cast<double>(m_ssmvar.turn_ratio) * dnm * static_cast<double>(m_ssmvar.output_full_load_res));

        return tmp*mult;
    }

    /**
     * @brief coDCMQualityFact - $Q$
     * @return
     */
    inline double coCCMQualityFact() const
    {
        double dnm1 = m_ssmvar.primary_ind
                /(qPow(static_cast<double>(m_ssmvar.turn_ratio), 2)
                * qPow((1-m_ssmvar.actual_duty), 2) * static_cast<double>(m_ssmvar.output_full_load_res));

        double dnm2 = m_ssmvar.output_cap_esr * static_cast<double>(m_ssmvar.output_full_load_res);

        return 1/(coCCMPoleTwoAngFreq()*(dnm1+dnm2));
    }

    /**
     * @brief coDCMDutyToInductCurrTrasfFunct - $G_{id}(s) magnitude value for current frequency$
     * @param freq
     * @return
     */
    double coMagCCMDutyToInductCurrTrasfFunct(double freq)
    {
        double num = qSqrt(1 + qPow((freq/coPoleOneAngFreq()), 2));

        double dnm = qSqrt(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2) + qPow((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())), 2));

        return coCCMCurrGainCoeff() * (num/dnm);
    }

    /**
     * @brief coPhsCCMDutyToInductCurrTrasfFunct - $G_{id}(s) phase value of current frequency$
     * @param freq
     * @return
     */
    double coPhsCCMDutyToInductCurrTrasfFunct(double freq)
    {
        double farg = qAtan(freq/coPoleOneAngFreq());
        double sarg = qAtan(
                            (freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())) *
                            (1/(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2)))
                           );
        return farg - sarg;
    }

    /********************CCM*************************/
    /********************F_m*************************/

    /**
     * @brief coCurrDetectSlopeVolt - $S_{n}$ - the voltage slope when the primary-side current is detected on RS_s
     *        The inductor rising slope.
     * @return
     */
    inline double coCurrDetectSlopeVolt() const
    {
        return (m_ssmvar.input_voltage * m_ssmvar.res_sense)
                / m_ssmvar.primary_ind;
    }

    /**
     * @brief coTimeConst - $\tau_{L}$ - switching period
     * @return
     */
    inline double coTimeConst() const
    {
        return (2 * m_ssmvar.primary_ind * static_cast<double>(m_ssmvar.freq_switch))
                /(qPow(m_ssmvar.turn_ratio, 2) * static_cast<double>(m_ssmvar.output_full_load_res));
    }

    /**
     * @brief coGainCurrModeContrModulator - $F_{m}$ - the PWM modulator gain
     * @return
     */
    inline double coGainCurrModeContrModulator() const
    {
        return 1/((coCurrDetectSlopeVolt()+m_ssmvar.sawvolt)*coTimeConst());
    }

    /********************F_m*************************/
    /********************OUT*************************/

    /**
     * @brief coMagDutyToOutTrasfFunct - $G_{vd}(s)$ -
     * @param freq
     * @return
     */
    double coMagDutyToOutTrasfFunct(double freq)
    {
        double result = 0.0;
        double num1 = 0.0;
        double num2 = 0.0;
        double dnm1 = 0.0;
        double dnm2 = 0.0;
        if(m_mode == DCM_MODE)
        {
            num1 = qSqrt(1 + qPow((freq/coZeroOneAngFreq()), 2));
            num2 = qSqrt(1 - qPow((freq/coDCMZeroTwoAngFreq()), 2));
            dnm1 = qSqrt(1 + qPow((freq/coPoleOneAngFreq()), 2));
            dnm2 = qSqrt(1 + qPow((freq/coDCMPoleTwoAngFreq()), 2));
            result = coDCMCriticValue()*((num1*num2)/(dnm1*dnm2));
        }
        else if(m_mode == CCM_MODE)
        {
            num1 = qSqrt(1 + qPow((freq/coCCMZeroTwoAngFreq()), 2));
            num2 = qSqrt(1 + qPow((freq/coZeroOneAngFreq()), 2));
            dnm1 = qSqrt(
                        qPow(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2), 2) +
                        qPow((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())), 2)
                        );
            result = coCCMVoltGainCoeff()*((num1*num2)/(dnm1));
        }
        return result;
    }

    /**
     * @brief coPhsDutyToOutTrasfFunct - $G_{vd}(s)$ -
     * @param freq
     * @return
     */
    double coPhsDutyToOutTrasfFunct(double freq)
    {
        double result = 0.0;
        double farg = qAtan(freq/coCCMZeroTwoAngFreq());
        double sarg = qAtan(freq/coZeroOneAngFreq());
        double targ = 0.;
        if(m_mode == DCM_MODE)
        {
            targ = qAtan(freq/coPoleOneAngFreq());
            result = farg - sarg - targ - qAtan(freq/coDCMPoleTwoAngFreq());
        }
        else if(m_mode == CCM_MODE)
        {
            targ = qAtan((freq/(coCCMQualityFact()*coCCMPoleTwoAngFreq())) * (1/(1 - qPow((freq/coCCMPoleTwoAngFreq()), 2))));
            result = farg - sarg - targ;
        }
        return result;
    }

    /**
     * @brief coMagControlToOutTransfFunct - $G_{vc}(s)$ - The control-to-output transfer function
     * @param freq
     * @return
     */
    double coMagControlToOutTransfFunct(double freq)
    {
        double result = 0.0;
        double dnm = 0.0;
        if(m_mode == DCM_MODE)
        {
            result = 20 * log10(coMagDutyToOutTrasfFunct(freq) * coGainCurrModeContrModulator());
        }
        else if(m_mode == CCM_MODE)
        {
            dnm = 20 * log10(coGainCurrModeContrModulator() * m_ssmvar.res_sense) * coMagCCMDutyToInductCurrTrasfFunct(freq);
            result = ( 20 * log10(coGainCurrModeContrModulator()) * coMagDutyToOutTrasfFunct(freq))/(1+dnm);
        }
        return result;

    }

    double coPhsControlToOutTransfFunct(double freq)
    {
        double result = 0.0;
        double dnm = 0.0;
        if(m_mode == DCM_MODE)
        {
            result = coPhsDutyToOutTrasfFunct(freq) * (M_PI_DEG/M_PI);
        }
        else if(m_mode == CCM_MODE)
        {
            dnm = qAtan(coGainCurrModeContrModulator() * m_ssmvar.res_sense) * coMagCCMDutyToInductCurrTrasfFunct(freq);
            result = (qAtan(coGainCurrModeContrModulator()) * coMagDutyToOutTrasfFunct(freq))/(1+dnm);
        }
        return result;

    }

    void coGainControlToOutTransfFunct(QVector<double> &in_freq, QVector<double> &out_mag)
    {
        auto itr_freq = in_freq.begin();
        out_mag.reserve(in_freq.size());
        double frq = 0., result = 0.;

        while(itr_freq != in_freq.end())
        {
            frq = *itr_freq;
            result = coMagControlToOutTransfFunct(frq);
            out_mag.push_back(result);
            itr_freq++;
        }
    }

    void coPhaseControlToOutTransfFunct(QVector<double> &in_freq, QVector<double> &out_phase)
    {
        auto itr_freq = in_freq.begin();
        out_phase.reserve(in_freq.size());
        double frq = 0., result = 0.;

        while(itr_freq != in_freq.end())
        {
            frq = *itr_freq;
            result = coPhsControlToOutTransfFunct(frq);
            out_phase.push_back(result);
            itr_freq++;
        }
    }

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

class FCCD
{
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
    FCCD(FCPreDesign &fcvar, RampSlopePreDesign &rsvar, LCSecondStage &lcfvar, PS_MODE mode = DCM_MODE)
    {
        qSwap(m_fcvar, fcvar);
        qSwap(m_rsvar, rsvar);
        qSwap(m_lcfvar, lcfvar);
        m_mode = mode;
    }

    /**
     * @brief coFreqCrossSection - f_{cross}
     * @return
     */
    double coFreqCrossSection() const
    {
        double ratio_factor = qPow((m_rsvar.prim_turns/m_rsvar.sec_turns_to_control), 2);

        double coeff = 1./5.;

        double num = (qPow(m_fcvar.out_voltage, 2)/
                      m_rsvar.out_pwr_tot) * qPow(m_rsvar.actual_duty, 2);

        double dnm = 2 * M_PI * m_rsvar.primary_ind;

        return (num/dnm)*ratio_factor*coeff;
    }

    /**
     * @brief coFreqPole - f_{pole}
     * @return
     */
    inline double coFreqPole() const
    {
        return (qTan(coBoost()) + qSqrt(qPow(qTan(coBoost()), 2) + 1)) * coFreqCrossSection();
    }

    /**
     * @brief coFreqZero - f_{zero}
     * @return
     */
    inline double coFreqZero() const
    {
        return qPow(coFreqCrossSection(), 2)/coFreqPole();
    }

    //1.
    /**
     * @brief coResOptoDiode - R_{LED_max}
     * @return
     */
    double coResOptoDiode() const
    {
        double num = static_cast<double>(m_fcvar.out_voltage) - S_OPTO_FORVARD_DROP - S_TL431_VREF;

        double dnm = S_INT_BIAS_CONTR - S_OPTO_CE_SAT + (S_TL431_CURR_CATH * m_fcvar.opto_ctr * m_fcvar.res_pull_up);

        return (num / dnm) * (m_fcvar.opto_ctr * m_fcvar.res_pull_up) * 0.15;//15% marg
    }

    //2.
    /**
     * @brief coResUp - R_{1} or R_{up} in voltage divider K_{d}
     * @return
     */
    inline double coResUp() const
    {
        return m_fcvar.res_down * (m_fcvar.out_voltage - S_TL431_VREF) / S_TL431_VREF;
    }

    //3.
    /**
     * @brief coResOptoBias - R_{bias}
     * @return
     */
    double coResOptoBias() const
    {
        double num = S_OPTO_FORVARD_DROP + (coResOptoDiode() * ((S_INT_BIAS_CONTR - 3)/
                                                            (m_fcvar.opto_ctr * m_fcvar.res_pull_up)));
        return num / (S_TL431_CURR_CATH);
    }

    //4.
    /**
     * @brief coVoltageOptoGain - K_{c} or G_{0} in Db 20*log10(K_{c})
     * @return
     */
    inline double coVoltageOptoGain() const
    {
        return (m_fcvar.res_pull_up/coResOptoDiode()) * m_fcvar.opto_ctr;
    }

    //5.
    /**
     * @brief coResZero - R_{f} or R_{zero}
     * @return
     */
    double coResZero() const
    {
        /*
        double gplant = 0.;
        double pz_ratio = coFreqPole()/coFreqZero();
        double t_ratio = static_cast<double>(m_rsvar.sec_turns_to_control)/m_rsvar.prim_turns;
        double ind_coeff = m_rsvar.inp_voltage * m_rsvar.res_sense * m_rsvar.primary_ind;

        if(m_mode == DCM_MODE){
            gplant = pz_ratio * qSqrt((m_rsvar.primary_ind * m_fcvar.freq_sw * m_fcvar.out_voltage)/(8 * m_fcvar.out_current)) *
                    ((m_rsvar.inp_voltage)/ind_coeff);
        }
        else if(m_mode == CCM_MODE){
            gplant = pz_ratio * ((qPow(m_rsvar.inp_voltage, 2) * m_fcvar.out_voltage) /
                              (2 * m_fcvar.out_current * (2 * m_rsvar.inp_voltage + t_ratio * m_fcvar.out_voltage) * ind_coeff));
        }

        double coeff = qSqrt(qPow((coFreqCrossSection()/coFreqPole()), 2) + 1)/
                       qSqrt(qPow((coFreqZero()/coFreqCrossSection()), 2) + 1);

        return gplant * coResUp() * coeff;
        */
        double gain = (1 / m_fcvar.res_pull_up) * (coResOptoDiode() / m_fcvar.opto_ctr);
        double coeff = (gain * coFreqCrossSection() * coResUp()) / coFreqPole();

        double fr_coeff = qSqrt(((qPow(coFreqZero(), 2) + qPow(coFreqCrossSection(), 2)) * (qPow(coFreqPole(), 2) + qPow(coFreqCrossSection(), 2)))) / (qPow(coFreqZero(), 2) + qPow(coFreqCrossSection(), 2));

        return fr_coeff * coeff;
    }

    //6.
    /**
     * @brief coCapZero - C_{f} or C_{zero}
     * @return
     */
    inline double coCapZero() const
    {
        return 1/(2 * M_PI * coFreqZero() * coResZero());
    }

    //7.
    /**
     * @brief coCapPoleOpto - C_{opto} - pulldown capacitor in hight optocoupler side
     * @return
     */
    inline double coCapPoleOpto() const
    {
        return 1/(2 * M_PI * coFreqPole() * m_fcvar.res_pull_up);
    }

    //8.
    inline double coResDivideGain() const
    {
        return coResZero()/coResUp();
    }

    //9.
    /**
     * @brief coTransfZero - $\f_{z}$
     * @return
     */
    inline double coTransfZero() const
    {
        return 1/(2 * M_PI * coResZero() * coCapZero());
    }

    /**
     * @brief coTransfPoleOne - $\f_{p1}$
     * @return
     */
    inline double coTransfPoleOne() const
    {
        return 1/(2 * M_PI * m_fcvar.res_pull_up * coCapPoleOpto());
    }

    //10.
    /**
     * @brief coTranfRCZero - \omega_{zesr}
     * @return
     */
    inline double coTranfRCZero() const
    {
        return 1/(2 * M_PI * m_fcvar.out_sm_cap_esr * m_fcvar.out_sm_cap);
    }

    /**
     * @brief coTransfLCZero - \omega_{op}
     * @return
     */
    inline double coTransfLCZero() const
    {
        /*
        double lc_ratio = 1/qSqrt(m_lcfvar.lcf_ind * m_lcfvar.lcf_cap);
        double load_ratio = qSqrt((m_fcvar.out_voltage/m_fcvar.out_current)/(m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current)));
        return lc_ratio * load_ratio;
        */
        //return 1/(2 * M_PI * qSqrt(m_lcfvar.lcf_ind * m_lcfvar.lcf_cap));
        return qSqrt((m_lcfvar.lcf_cap_esr + m_lcfvar.lcf_cap) / ( m_lcfvar.lcf_ind * m_lcfvar.lcf_cap_esr * m_lcfvar.lcf_cap));
    }

    /**
     * @brief coQualityLC - Q_{lc}
     * @return
     */
    double coQualityLC() const
    {
        /*
        double num = m_lcfvar.lcf_ind * m_lcfvar.lcf_cap * coTransfLCZero() * (m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current));
        double dnm = m_lcfvar.lcf_ind + m_lcfvar.lcf_cap * (m_lcfvar.lcf_cap_esr + (m_fcvar.out_voltage/m_fcvar.out_current));

        return num/dnm;
        */
        //double r_load = m_fcvar.out_voltage/m_fcvar.out_current;
        //double dnm = (1/r_load) * (qSqrt(m_lcfvar.lcf_ind / m_lcfvar.lcf_cap) + (m_fcvar.out_sm_cap_esr + m_lcfvar.lcf_cap_esr) + qSqrt(m_lcfvar.lcf_cap / m_lcfvar.lcf_ind));

        //return 1 / dnm;
        return (m_fcvar.out_voltage / m_fcvar.out_current) / m_lcfvar.lcf_ind;
    }

    /////////////////////////////////////////////
    /**
     * @brief coIndOnTimeSlope - S_{n}
     * @return
     */
    inline double coIndOnTimeSlope() const
    {
        return (m_rsvar.inp_voltage * m_rsvar.res_sense)/m_rsvar.primary_ind;
    }

    /**
     * @brief FCCD::coCompRamp - M_{c}
     * @return
     */
    double coCompRamp() const
    {
        double coeff = m_rsvar.prim_turns/m_rsvar.sec_turns_to_control;

        return (coeff  * m_fcvar.out_voltage)/m_rsvar.inp_voltage;
    }

    /**
     * @brief FCCD::coExterRampSlope - S_{e}
     * @return
     */
    inline double coExterRampSlope() const
    {
        return 0.5 * ((m_fcvar.out_voltage / ((m_rsvar.prim_turns/m_rsvar.sec_turns_to_control) * m_rsvar.primary_ind)) * m_rsvar.res_sense);
        //return (coCompRamp()-1)*coIndOnTimeSlope();
    }

    /**
     * @brief coQuality - Q_{p}
     * @return
     */
    double coQuality() const
    {
        double inv_duty = 1 - m_rsvar.actual_duty;
        return 1/(M_PI*(coCompRamp()*inv_duty-0.5));
    }

    /////////////////////////////////////////////
    /**
     * @brief coLCTransfFunc - |H_{lc}(s)|
     * @param s
     * @return
     */
    double coMagLCTransfFunc(double freq) const
    {
        //double omega_zero = qPow(1 - qPow((freq/coTransfLCZero()), 2), 2);
        //double qual = qPow((freq/coQualityLC() * coTransfLCZero()), 2);

        double omega_zer1 = qSqrt(1 + qPow((freq/coTranfRCZero()), 2));
        double dnm = qSqrt(qPow((1 - qPow((freq / coTransfLCZero()), 2)), 2) + qPow((freq / (coQualityLC() * coTransfLCZero())), 2));

        return omega_zer1 * (1/dnm);
    }

    /**
     * @brief coOptoFeedbTransfFunc - |H(s)|
     * @param s
     * @return
     */
    inline double coMagOptoFeedbTransfFunc(double freq) const
    {
        double zero_one = qSqrt(1 + qPow((coTransfZero()/freq), 2));
        double pole_one = qSqrt(1 + qPow((freq/coTransfPoleOne()), 2));
        double g_0 = coVoltageOptoGain() * coResDivideGain();

        return g_0 * (zero_one / pole_one) * coMagLCTransfFunc(freq);
    }

    /**
     * @brief coPhsLCTransfFunc - /_ H_{lc}(s)
     * @param freq
     * @return
     */
    inline double coPhsLCTransfFunc(double freq) const
    {
        return qAtan(freq/coTranfRCZero()) - qAtan((freq/(coTransfLCZero() * coQualityLC())) * (1/(1 - qPow((freq/coTransfLCZero()), 2))));
    }

    /**
     * @brief coPhsOptoFeedbTransfFunc - /_ H(s)
     * @param freq
     * @return
     */
    inline double coPhsOptoFeedbTransfFunc(double freq) const
    {
        return qAtan(freq/coTransfZero())-qAtan(freq/coTransfPoleOne());
    }

    //7.
    /**
     * @brief coGainOptoFeedbTransfFunc - create the 20*log_10(|H(s)|) sequence values
     * @param freq
     * @return
     */
    void coGainOptoFeedbTransfFunc(QVector<double> &in_freq, QVector<double> &out_mag)
    {
        auto itr_freq = in_freq.begin();
        out_mag.reserve(in_freq.size());
        double frq = 0., result = 0.;

        while(itr_freq != in_freq.end())
        {
            frq = *itr_freq;
            result = 20 * log10(coMagOptoFeedbTransfFunc(frq));
            out_mag.push_back(result);
            itr_freq++;
        }
    }

    /**
     * @brief coPhaseOptoFeedbTransfFunc
     * @param freq
     * @return
     */
    void coPhaseOptoFeedbTransfFunc(QVector<double> &in_freq, QVector<double> &out_phase)
    {
        auto itr_freq = in_freq.begin();
        out_phase.reserve(in_freq.size());
        double frq = 0., result = 0.;

        while (itr_freq != in_freq.end())
        {
            frq = *itr_freq;
            result = (coPhsOptoFeedbTransfFunc(frq) - coPhsLCTransfFunc(frq))  * (M_PI_DEG/M_PI);
            out_phase.push_back(result);
            itr_freq++;
        }
    }
};
#endif // CONTROLOUT_H
