#include "fbptransformer.h"

FBPTransformer::FBPTransformer()
{

}
/*Inductance of primary side*/
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::DutyCycleDCM(const InputValue &ivalue, const BCap &bcvalue)
{
    return  ivalue.refl_volt_max/((ivalue.refl_volt_max)+(bcvalue.input_dc_min_voltage));
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::InputPower(const InputValue &ivalue)
{
    return (ivalue.power_out_max)/(ivalue.eff);
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::PriInduct(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue)
{
    return ((bcvalue.input_dc_min_voltage * fbptval.max_duty_cycle) * (bcvalue.input_dc_min_voltage * fbptval.max_duty_cycle))/(2. * fbptval.inp_power*ivalue.freq_switch * KRF);
}
/*Inductance of primary side*/

/** All current primary side*/
/**
  * @brief Primary average current during turn-on
  * @param inp_power -
  * @param input_min_voltage -
  * @param max_duty_cycle -
  * @return average current
  */
double FBPTransformer::CurrPriAver(const BCap &bcvalue, const FBPT &fbptval)
{
    return fbptval.inp_power/((bcvalue.input_min_voltage)*(fbptval.max_duty_cycle));
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::CurrPriPeakToPeak(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue)
{
    return ((bcvalue.input_dc_min_voltage)*(fbptval.max_duty_cycle))/((fbptval.primary_induct)*(ivalue.freq_switch));
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::CurrPriMax(const FBPT &fbptval)
{
    return  (fbptval.curr_primary_aver)+((fbptval.curr_primary_peak_peak)/2);
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::CurrPriValley(const FBPT &fbptval)
{
    return (fbptval.curr_primary_peak)-(fbptval.curr_primary_peak_peak);
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTransformer::CurrPriRMS(const FBPT &fbptval)
{
    return sqrt((3.*(fbptval.curr_primary_aver * fbptval.curr_primary_aver)+((fbptval.curr_primary_peak_peak/2.)*(fbptval.curr_primary_peak_peak/2.)))*(fbptval.max_duty_cycle/3.));
}
/*All current primary side*/
