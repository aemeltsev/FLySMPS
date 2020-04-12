/**
  * License text...
*/
#include "diodebridge.h"

DiodeBridge::DiodeBridge()
{

}
/**
  * @brief  Calculate the diode peak current
  * @param  bcapacitor_peak_curr - bulk capacitor peak current
  * @param  load_curr_max - load maximum current
  * @retval IDiodePeak - diode peak current
  */
double DiodeBridge::IDiodePeak(BCap* bcvalue)
{
    return bcvalue->bcapacitor_peak_curr+bcvalue->load_curr_max;
}
/**
  * @brief  Root mean square value of current diode
  * @param  load_avg_curr - diode average current
  * @param  freq_line - frequency in power line
  * @param  diode_cond_time - diode total conduction time
  * @retval IDiodeRMS - rms current diode
  */
double DiodeBridge::IDiodeRMS(InputValue* ivalue, DBridge* dbvalue)
{
    return (dbvalue->load_avg_curr)/(sqrt(3*(ivalue->freq_line)*(dbvalue->diode_cond_time)));
}
/**
  * @brief  Calculate diode average current
  * @param  load_avg_curr - average dc current
  * @retval IDiodeAVG - average current value
  */
double DiodeBridge::IDiodeAVG(DBridge* dbvalue)
{
    return (dbvalue->load_avg_curr)/2;
}
/**
  * @brief  Total root mean square value of current diode
  * @param  load_avg_curr - diode average current
  * @param  freq_line - frequency in power line
  * @param  diode_cond_time - diode total conduction time
  * @retval IDiodeRMSTot - total rms current diode
  */
double DiodeBridge::IDiodeRMSTot(DBridge* dbvalue, InputValue* ivalue)
{
    return ((dbvalue->load_avg_curr)*sqrt(2))/(sqrt(3*(ivalue->freq_line)*(dbvalue->diode_cond_time)));
}
/**
  * @brief  Calculate the average dc current
  * @param  diode_peak_curr - diode peak current
  * @param  freq_line - frequency in power line
  * @param  diode_cond_time - diode total conduction time
  * @retval ILoadAVG - average dc current
  */
double DiodeBridge::ILoadAVG(DBridge* dbvalue, InputValue* ivalue)
{
    return (dbvalue->diode_peak_curr)*(ivalue->freq_line)*(dbvalue->diode_cond_time);
}
/**
  * @brief  The diode total conduction time
  * @param  diode_peak_curr - diode peak current
  * @param  diode_curr_slope - diode current down slope
  * @retval DiodeConductTime - total conduction time for diode
  */
double DiodeBridge::DiodeConductTime(DBridge* dbvalue)
{
    return (dbvalue->diode_peak_curr)/(dbvalue->diode_curr_slope);
}
/**
  * @brief  Diode current down slope from the peak value to total charging time
  * @param  diode_peak_curr - diode peak current
  * @param  load_curr_min - load peak current value
  * @param  charg_time - total charging time
  * @retval DiodeCurrentSlope - Diode current down slope, in A/s
  */
double DiodeBridge::DiodeCurrentSlope(DBridge* dbvalue, BCap* bcvalue)
{
    return ((dbvalue->diode_peak_curr)-(bcvalue->load_curr_min))/(bcvalue->charg_time);
}
/**
  * @brief  Solve minimum peak value
  * @param  input_volt_ac_min - minimum ac line amplitude value
  * @retval MinPeakInVoltage - minimum peak value
  */
double DiodeBridge::MinPeakInVoltage(InputValue* ivalue)
{
    return ivalue->input_volt_ac_min*sqrt(2);
}
/**
  * @brief  Solve maximum peak value
  * @param  input_volt_ac_max - maximum ac line amplitude value
  * @retval MaxPeakInVoltage - maximum peak value
  */
double DiodeBridge::MaxPeakInVoltage(InputValue* ivalue)
{
    return  ivalue->input_volt_ac_max*sqrt(2);
}
