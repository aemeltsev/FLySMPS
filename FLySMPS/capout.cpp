#include "capout.h"

/**
  * @brief select output ESR based on the allowable output ripple voltage
  * @return return equivalent series resistance
  */
inline double CapOut::ocESRCapOut()
{
    return (volts_rippl*esr_perc)/curr_peak_out;
}
/**
  * @brief time charging
  * @param freq_switch - operatin switch frequency
  * @return charg time
  */
inline double CapOut::ocTimeCapCharg(const InputValue &ivalue)
{
    double cross_frq = cros_frq_start_val*ivalue.freq_switch;
    return (1/(4*cross_frq))+(1/ivalue.freq_switch);
}
/**
  * @brief peak-to-peak ripple current value
  * @return current p-t-p
  */
inline double CapOut::ocCurrCap()
{
    return curr_peak_out/2;
}
/**
  * @brief design the minimun output capacitor value
  * @param ivalue for time charging calculation
  * @return output capacitor
  */
inline double CapOut::ocCapOutValue(const InputValue& ivalue)
{
    return (ocCurrCap()*ocTimeCapCharg(ivalue))/(2*(volts_out*volts_rippl));
}
/**
  * @brief estimate total output capacitor RMS current
  * @param actual_max_duty_cycle actual switching duty cycle
  * @return rms current
  */
inline double CapOut::ocCurrOurRMS(const FBPT &fbptval)
{
    return curr_peak_out*sqrt((fbptval.actual_max_duty_cycle/3))-curr_peak_out;
}
/**
 * @brief CapOut::ocCapOutLoss - estimate total output capacitor loss
 * @param fbptval - actual_max_duty_cycle for rms current
 * @return losses
 */
inline double CapOut::ocCapOutLoss(const FBPT &fbptval)
{
    return pow(ocCurrOurRMS(fbptval), 2)*ocESRCapOut();
}
/**
 * @brief CapOut::ocZeroFreqCapOut
 * @param ivalue
 * @return
 */
inline double CapOut::ocZeroFreqCapOut(const InputValue& ivalue)
{
    return 1./(2.*S_PI*ocESRCapOut()*ocCapOutValue(ivalue));
}
/**
 * @brief CapOut::ocOutRippleVolt
 * @param curout
 * @param ncap
 * @return
 */
inline double CapOut::ocOutRippleVolt(double curout, int16_t ncap)
{
    return (curout*ocESRCapOut())/ncap;
}
