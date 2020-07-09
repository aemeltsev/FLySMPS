#include "capout.h"

/**
  * @brief select output ESR based on the allowable output ripple voltage
  * @param
  * @return return equivalent series resistance
  */
double CapOut::ocESRCapOut()
{
    return (volts_rippl*esr_perc)/curr_peak_out;
}
/**
  * @brief time charging
  * @param freq_switch - operatin switch frequency
  * @return charg time
  */
double CapOut::ocTimeCapCharg(const InputValue &ivalue)
{
    double cross_frq = cros_frq_start_val*ivalue.freq_switch;
    return (1/(4*cross_frq))+(1/ivalue.freq_switch);
}
/**
  * @brief peak-to-peak ripple current value
  * @param
  * @return current p-t-p
  */
double CapOut::ocCurrCap()
{
    return curr_peak_out/2;
}
/**
  * @brief design the minimun output capacitor value
  * @param ivalue for time charging calculation
  * @return output capacitor
  */
double CapOut::ocCapOutValue(const InputValue& ivalue)
{
    return (ocCurrCap()*ocTimeCapCharg(ivalue))/(2*(volts_out*volts_rippl));
}
/**
  * @brief estimate total output capacitor RMS current
  * @param actual_max_duty_cycle actual switching duty cycle
  * @return rms current
  */
double CapOut::ocCurrOurRMS(const FBPT &fbptval)
{
    return curr_peak_out*sqrt((fbptval.actual_max_duty_cycle/3))-curr_peak_out;
}
/**
  * @brief estimate total output capacitor loss
  * @param actual_max_duty_cycle for rms current
  * @return losses
  */
double CapOut::ocCapOutLoss(const FBPT &fbptval)
{
    return pow(ocCurrOurRMS(fbptval), 2)*ocESRCapOut();
}
