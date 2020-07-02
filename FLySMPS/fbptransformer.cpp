#include "fbptransformer.h"

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
    return pow((bcvalue.input_dc_min_voltage * fbptval.max_duty_cycle), 2)/(2. * fbptval.inp_power*ivalue.freq_switch * ripple_factor);
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
  * @return current rms value
  */
double FBPTransformer::CurrPriRMS(const FBPT &fbptval)
{
    return sqrt((3.*(fbptval.curr_primary_aver * fbptval.curr_primary_aver)+((fbptval.curr_primary_peak_peak/2.)*(fbptval.curr_primary_peak_peak/2.)))*(fbptval.max_duty_cycle/3.));
}
/*All current primary side*/

/*Area product calculation*/
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::EnergyStoredChoke(const FBPT &fbptval)
{
    return (fbptval.primary_induct * pow(fbptval.curr_primary_peak, 2))/2.;
}
/**
  * @brief Estimate the core geometry coefficient
  * @param
  * @return
  */
double FBPTCore::CoreAreaProd(const FBPT &fbptval)
{
    return (2. * EnergyStoredChoke(fbptval))/(curr_dens * core_win_util_fact * flux_dens_max);
}
/**
  * @brief
  * @param
  * @return
  */
/*
double FBPTCore::DeltaFluxMax(const FBPT &fbptval)
{
    return  flux_dens_max * (fbptval.curr_primary_peak_peak/(1.1*(fbptval.curr_primary_peak_peak)));
}
*/
/**
  * @brief Calculate windows area product AP
  * @param use from FPBT struct - primary inductance, rms and peak current
  * @return area product ratio
  */
double FBPTCore::CoreWinToCoreSect(const FBPT &fbptval)
{
    double tmp = (fbptval.primary_induct * fbptval.curr_primary_rms * fbptval.curr_primary_peak)/(flux_dens_max * S_K_1);
    return pow(tmp, (4./3.));
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::AreaWindTotal(const FBPT &fbptval, const InputValue &ivalue, const CoreSelection &cs)
{
    double tmp = core_win_util_fact * cs.core_wind_area * S_RO_OM * cs.mean_leng_per_turn;
    return fbptval.curr_primary_peak * sqrt(tmp/ivalue.power_out_max);
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::CurrentDens(const FBPT &fbptval)
{
    return fbptval.curr_primary_peak/fbptval.area_wind_tot;
}
/**
  * @brief
  * @param
  * @return
  */
void FBPTCore::setCoreSelection(double ap, double mu_rc,
                                double ac, double wa,
                                double vc, double lt,
                                double lc, double hw, double al,
                                CoreSelection &cs)
{
 ap = cs.core_area_product;
 mu_rc = cs.core_permeal;
 ac = cs.core_cross_sect_area;
 wa = cs.core_wind_area;
 vc = cs.core_vol;
 lt = cs.mean_leng_per_turn;
 lc = cs.mean_mag_path_leng;
 hw = cs.core_win_height;
 al = cs.ind_fact;
}
/*Area product calculation*/
