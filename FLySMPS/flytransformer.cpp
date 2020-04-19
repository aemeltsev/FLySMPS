#include "flytransformer.h"

FlyTransformer::FlyTransformer()
{

}

double FlyTransformer::DutyCycleDCM(InputValue *ivalue, BCap *bcvalue)
{
    return ivalue->refl_volt_max/((ivalue->refl_volt_max)+(bcvalue->input_dc_min_voltage));
}
double FlyTransformer::InputPower(InputValue *ivalue)
{
    return (ivalue->power_out_max)/(ivalue->eff);
}
double FlyTransformer::PriInduct(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue)
{
    return ((bcvalue->input_dc_min_voltage*fbtvalue->max_duty_cycle)*(bcvalue->input_dc_min_voltage*fbtvalue->max_duty_cycle))/(2*fbtvalue->inp_power*ivalue->freq_switch*fbtvalue->KRF);
}
