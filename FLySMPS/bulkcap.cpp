////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#include "bulkcap.h"
#define PI 3.1415926535897932385

BulkCap::BulkCap()
{

}

/**
  * @brief The total refueling time from Vmin to Vpeak
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @retval DeltaT - Calculating time value from Vmin to Vpeak
  */
double BulkCap::DeltaT(InputValue* ivalue)
{
    double volt_rrms = ((ivalue->input_volt_ac_min)*(1/(sqrt(2))))-((ivalue->input_volt_ac_min)*(2/PI));
    double volt_out = ((ivalue->input_volt_ac_min)*sqrt(2))-volt_rrms;
    return (asin(volt_out/(ivalue->input_volt_ac_min*sqrt(2))))/(2*PI*(ivalue->freq_line));
}

/**
  * @brief The total charging time
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @retval ChargTime - total charging time value
  */
double BulkCap::ChargTime(InputValue* ivalue)
{ 
    return ((1/(4*(ivalue->freq_line)))-(DeltaT(ivalue)));
}

/**
  * @brief Calculate the bulk capacitor value
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @param POut - summary output power of the converter
  * @param Eff - efficiency
  * @retval CapValue - bulk capacitor value
  */
double BulkCap::CapValue(InputValue* ivalue)
{
    double volt_rrms = ((ivalue->input_volt_ac_min)*(1/(sqrt(2))))-((ivalue->input_volt_ac_min)*(2/PI));
    double volt_out = ((ivalue->input_volt_ac_min)*sqrt(2))-volt_rrms;
    return ((2*(ivalue->power_out_max))*(1/(4*ivalue->freq_line))+(DeltaT(ivalue)))/((ivalue->eff)*((volt_rrms*volt_rrms)-(volt_out*volt_out)));
}

/**
  * @brief Load peak current value
  * @param POut - summary output power of the converter
  * @param Eff - efficiency
  * @param VInMinRMS - Minimum RMS value line voltage
  * @retval ILoadMax - peak current value
  */
double BulkCap::ILoadMax(InputValue* ivalue, DBridge* dbvalue)
{
    return (ivalue->power_out_max)/((ivalue->eff)*(dbvalue->in_min_rms_voltage));
}

/**
  * @brief Load minimum current value
  * @param POut - summary output power of the converter
  * @param Eff - efficiency
  * @param VInMaxRMS - Maximum RMS value line voltage
  * @retval ILoadMin - minimum current value
  */
double BulkCap::ILoadMin(InputValue* ivalue, DBridge* dbvalue)
{
    return (ivalue->power_out_max)/((ivalue->eff)*(dbvalue->in_max_rms_voltage));
}

/**
  * @brief The bulk capacitor peak current
  * @param CapVal - bulk capacitor value
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @retval IBulkCapPeak - bulk capacitor peak current
  */
double BulkCap::IBulkCapPeak(BCap* bcvalue, InputValue* ivalue)
{
    double volt_rrms = ((ivalue->input_volt_ac_min)*(1/(sqrt(2))))-((ivalue->input_volt_ac_min)*(2/PI));
    double volt_out = ((ivalue->input_volt_ac_min)*sqrt(2))-volt_rrms;
    return (2*PI*(ivalue->freq_line)*(bcvalue->bcapacitor_value)*(volt_out)*(cos(2*PI*(ivalue->freq_line)*DeltaT(ivalue))));
}

/**
  * @brief Calculate bulk capacitor RMS current value
  * @param ILoadAVG - diode average current
  * @param DiodeConductTime - total conduction time for diode
  * @param FLine - frequency in power line
  * @retval IBulkCapRMS - bulk capacitor RMS current
  */
double BulkCap::IBulkCapRMS(DBridge *dbvalue, InputValue* ivalue)
{
    return dbvalue->load_avg_curr*(sqrt((2/(3*(ivalue->freq_line)*(dbvalue->diode_cond_time)))-1));
}

/**
  * @brief Recalculation after input capacitor selection
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param POut - summary output power of the converter
  * @param CapVal - bulk capacitor value
  * @param FLine - frequency in power line
  * @retval VMinInp - recalculation after input capacitor selection
  */
double BulkCap::VMinInp(BCap* bcvalue, InputValue* ivalue)
{
    double volt_rrms = ((ivalue->input_volt_ac_min)*(1/(sqrt(2))))-((ivalue->input_volt_ac_min)*(2/PI));
    double volt_out = ((ivalue->input_volt_ac_min)*sqrt(2))-volt_rrms;
    return sqrt((volt_out*volt_out)-((2*(ivalue->power_out_max)*((1/(4*ivalue->freq_line)-DeltaT(ivalue))))/(bcvalue->bcapacitor_value)));
}

/**
  * @brief VDCMin simply the average value of MinIng and VRectMinPeak
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param VMinInp - recalculation after input capacitor selection
  * @retval VDCMin - simply the average value of MinIng and VRectMinPeak
  */
double BulkCap::VDCMin(BCap* bcvalue, InputValue* ivalue)
{
    double volt_rrms = ((ivalue->input_volt_ac_min)*(1/(sqrt(2))))-((ivalue->input_volt_ac_min)*(2/PI));
    double volt_out = ((ivalue->input_volt_ac_min)*sqrt(2))-volt_rrms;
    return (volt_out+(bcvalue->input_min_voltage))/2;
}
