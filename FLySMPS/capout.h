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

#ifndef CAPOUT_H
#define CAPOUT_H
#include <QtMath>
#include <cstdint>

struct CapOutProp
{
    int16_t co_volts_out;//Secondary output voltage
    float co_curr_peak_out;//Secondary current peak
    float co_volts_rippl;//Ripple voltage of the secondary side
    float co_esr_perc;//ESR percentage(0.006–0.025 ohm)
    float co_cros_frq_start_val;//Crossover frequency(1/20 to 1/10 from working frequency)
};

/**
 * @brief The CapOut class
 *        See more:
 *        UCC28722-UCC28720 5W USB BJT Flyback Design Example
 *        MAXREFDES1036.-20.25W Offline Flyback Converter Using MAX17595
 */
class CapOut
{
private:
    CapOutProp m_cop;
    /**
     * @brief ocTimeResponse - time charging
     * @param freq_switch - operating switch frequency
     * @return charg time
     */
    inline double ocTimeResponse(uint32_t freq_switch) const
    {
        return (0.33/m_cop.co_cros_frq_start_val)+(1/freq_switch);
    }

    /**
     * @brief ocCurrStep - peak-to-peak ripple current value
     * @return current p-t-p
     */
    inline double ocCurrStep() const
    {
        return m_cop.co_curr_peak_out/2;
    }
public:
    CapOut(CapOutProp& cop)
    {
        qSwap(m_cop, cop);
    }

    /**
      * @brief select output ESR based on the allowable output ripple voltage
      * @return return equivalent series resistance
      */
    inline double ocESRCapOut() const
    {
        return static_cast<double>(((m_cop.co_volts_rippl * m_cop.co_volts_out) * m_cop.co_esr_perc)/m_cop.co_curr_peak_out);
    }

    /**
     * @brief ocCapOutValue - design the minimun output capacitor value
     * @param freq_switch - operating switch frequency
     * @return output capacitor
     */
    inline double ocCapOutValue(uint32_t freq_switch) const
    {
        double num = ocCurrStep() * ocTimeResponse(freq_switch);
        double dnm = static_cast<double>(m_cop.co_volts_out * m_cop.co_volts_rippl);
        return num / dnm;
    }

    /**
     * @brief ocCurrOurRMS - estimate total output capacitor RMS current
     * @param actual_max_duty_cycle - actual switching duty cycle
     * @return rms current in A
     */
    inline double ocCurrOurRMS(double curr_pri_peak, float trn_rat_curr) const
    {
        double num = 2. * curr_pri_peak;
        double dnm = 3. * trn_rat_curr * m_cop.co_curr_peak_out;
        return m_cop.co_curr_peak_out * qSqrt((num / dnm) - 1);
    }

    /**
     * @brief ocZeroFreqCapOut
     * @param freq_switch - operating switch frequency
     * @return
     */
    inline double ocZeroFreqCapOut(uint32_t freq_switch) const
    {
        return 1./(2.*M_PI*ocESRCapOut()*ocCapOutValue(freq_switch));
    }

    /**
     * @brief ocOutRippleVolt
     * @param curout
     * @param ncap
     * @return
     */
    inline double ocOutRippleVolt(double curr_pri_peak, double cap_out, float trn_rat, uint32_t freq_switch) const
    {
        double num = 4.5 * qPow((curr_pri_peak - (trn_rat * 4.5)), 2);
        double dnm = qPow(curr_pri_peak, 2) * freq_switch * cap_out;
        return num / dnm;
    }

    /**
     * @brief ocCapOutLoss - estimate total output capacitor loss
     * @param actual_max_duty_cycle  - actual_max_duty_cycle for rms current
     * @return losses in W
     */
    inline double ocCapOutLoss(double cur_cap_rms) const
    {
        return qPow(cur_cap_rms, 2)*ocESRCapOut();
    }
};

#endif // CAPOUT_H
