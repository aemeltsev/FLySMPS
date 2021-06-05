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

#include "capout.h"

/**
  * @brief select output ESR based on the allowable output ripple voltage
  * @return return equivalent series resistance
  */
inline double CapOut::ocESRCapOut() const
{
    return (m_cop.co_volts_rippl * m_cop.co_esr_perc)/m_cop.co_curr_peak_out;
}

/**
 * @brief ocTimeCapCharg - time charging
 * @param freq_switch - operating switch frequency
 * @return charg time
 */
inline double CapOut::ocTimeCapCharg(int16_t freq_switch) const
{
    double cross_frq = m_cop.co_cros_frq_start_val * freq_switch;
    return (1/(4*cross_frq))+(1/freq_switch);
}

/**
 * @brief ocCurrCap - peak-to-peak ripple current value
 * @return current p-t-p
 */
inline double CapOut::ocCurrCap() const
{
    return m_cop.co_curr_peak_out/2;
}

/**
 * @brief CapOut::ocCapOutValue - design the minimun output capacitor value
 * @param freq_switch - operating switch frequency
 * @return output capacitor
 */
inline double CapOut::ocCapOutValue(int16_t freq_switch) const
{
    return (ocCurrCap()*ocTimeCapCharg(freq_switch))/(2*(m_cop.co_volts_out * m_cop.co_volts_rippl));
}

/**
 * @brief ocCurrOurRMS - estimate total output capacitor RMS current
 * @param actual_max_duty_cycle - actual switching duty cycle
 * @return rms current in A
 */
inline double CapOut::ocCurrOurRMS(float actual_max_duty_cycle) const
{
    return m_cop.co_curr_peak_out * qSqrt((static_cast<double>(actual_max_duty_cycle)/3.)) - m_cop.co_curr_peak_out;
}

/**
 * @brief ocCapOutLoss - estimate total output capacitor loss
 * @param actual_max_duty_cycle  - actual_max_duty_cycle for rms current
 * @return losses in W
 */
inline double CapOut::ocCapOutLoss(float actual_max_duty_cycle) const
{
    return qPow(ocCurrOurRMS(actual_max_duty_cycle), 2)*ocESRCapOut();
}

/**
 * @brief ocZeroFreqCapOut
 * @param freq_switch - operating switch frequency
 * @return
 */
inline double CapOut::ocZeroFreqCapOut(int16_t freq_switch) const
{
    return 1./(2.*M_PI*ocESRCapOut()*ocCapOutValue(freq_switch));
}

/**
 * @brief ocOutRippleVolt
 * @param curout
 * @param ncap
 * @return
 */
inline double CapOut::ocOutRippleVolt(double curout, int16_t ncap) const
{
    return (curout*ocESRCapOut())/ncap;
}
