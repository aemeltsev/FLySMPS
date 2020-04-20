#include "flytransformer.h"

FlyTransformer::FlyTransformer()
{

}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::DutyCycleDCM(InputValue *ivalue, BCap *bcvalue)
{
    return ivalue->refl_volt_max/((ivalue->refl_volt_max)+(bcvalue->input_dc_min_voltage));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::InputPower(InputValue *ivalue)
{
    return (ivalue->power_out_max)/(ivalue->eff);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::PriInduct(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue)
{
    return ((bcvalue->input_dc_min_voltage*fbtvalue->max_duty_cycle)*(bcvalue->input_dc_min_voltage*fbtvalue->max_duty_cycle))/(2.*fbtvalue->inp_power*ivalue->freq_switch*KRF);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriAver(BCap *bcvalue, FBTransformer *fbtvalue)
{
    return fbtvalue->inp_power/((bcvalue->input_min_voltage)*(fbtvalue->max_duty_cycle));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriPeakToPeak(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue)
{
    return ((bcvalue->input_dc_min_voltage)*(fbtvalue->max_duty_cycle))/((fbtvalue->primary_induct)*(ivalue->freq_switch));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriMax(FBTransformer *fbtvalue)
{
    return  (fbtvalue->curr_primary_aver)+((fbtvalue->curr_primary_peak_peak)/2);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriValley(FBTransformer *fbtvalue)
{
    return (fbtvalue->curr_primary_peak)-(fbtvalue->curr_primary_peak_peak);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriRMS(FBTransformer *fbtvalue)
{
    return sqrt((3.*(fbtvalue->curr_primary_aver*fbtvalue->curr_primary_aver)+((fbtvalue->curr_primary_peak_peak/2.)*(fbtvalue->curr_primary_peak_peak/2.)))*(fbtvalue->max_duty_cycle/3.));
}

