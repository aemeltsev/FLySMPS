#include "fbptransformer.h"

FBPTransformer::FBPTransformer()
{

}
/*Inductance of primary side*/
/**
  * @brief Maximum duty cycle
  * @param from Input parameters - reflected voltage
  * @param from Bulk Cap struct - dc average voltage
  * @return duty cycle ratio
  */
double FBPTransformer::DutyCycleDCM(const InputValue &ivalue, const BCap &bcvalue)
{
    return  ivalue.refl_volt_max/((ivalue.refl_volt_max)+(bcvalue.input_dc_min_voltage));
}
/**
  * @brief Maximum input power
  * @param from Input parameters - full output power and efficiency
  * @return in pwr in W
  */
double FBPTransformer::InputPower(const InputValue &ivalue)
{
    return (ivalue.power_out_max)/(ivalue.eff);
}
/**
  * @brief Primary inductance
  * @param from FBPT struct - duty cycle and input power
  * @param from Bulk Cap struct - dc average, between min input and rectify min peak
  * @param from Input parameters - switching frequency
  * @return inductance in H
  */
double FBPTransformer::PriInduct(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue)
{
    return pow((bcvalue.input_dc_min_voltage * fbptval.max_duty_cycle), 2)/(2. * fbptval.inp_power*ivalue.freq_switch * KRF);
}
/*Inductance of primary side*/

/** All current primary side*/
/**
  * @brief Primary average current during turn-on
  * @param from FBPT struct - maximum input power and duty cycle value
  * @param from Bulk Cap struct - minimum voltage after the input capacitor
  * @return average current
  */
double FBPTransformer::CurrPriAver(const BCap &bcvalue, const FBPT &fbptval)
{
    return fbptval.inp_power/((bcvalue.input_min_voltage)*(fbptval.max_duty_cycle));
}
/**
  * @brief Primary peak-to-peak current
  * @param from Bulk Cap struct - dc average voltage
  * @param from FBPT struct - duty cycle and inductance
  * @param from Input parameters - switching frequency
  * @return primare delta current
  */
double FBPTransformer::CurrPriPeakToPeak(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue)
{
    return ((bcvalue.input_dc_min_voltage)*(fbptval.max_duty_cycle))/((fbptval.primary_induct)*(ivalue.freq_switch));
}
/**
  * @brief Primary peak current
  * @param from FBPT struct - average and delta current
  * @return peak value of primary current
  */
double FBPTransformer::CurrPriMax(const FBPT &fbptval)
{
    return  (fbptval.curr_primary_aver)+((fbptval.curr_primary_peak_peak)/2);
}
/**
  * @brief Primary valley current
  * @param from FBPT struct - peak current and delta current
  * @return the valley of the inductor current
  */
double FBPTransformer::CurrPriValley(const FBPT &fbptval)
{
    return (fbptval.curr_primary_peak)-(fbptval.curr_primary_peak_peak);
}
/**
  * @brief Primary RMS current
  * @param from FBPT struct - average, delta current and duty cycle ratio
  * @return rms value
  */
double FBPTransformer::CurrPriRMS(const FBPT &fbptval)
{
    return sqrt((3.*(fbptval.curr_primary_aver * fbptval.curr_primary_aver)+((fbptval.curr_primary_peak_peak/2.)*(fbptval.curr_primary_peak_peak/2.)))*(fbptval.max_duty_cycle/3.));
}
/*All current primary side*/
