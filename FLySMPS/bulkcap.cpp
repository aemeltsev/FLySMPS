////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#include "bulkcap.h"
#define PI 3.1415926535897932385

//Constructor
BulkCap::BulkCap()
{

}

/**
  * @brief The total refueling time from Vmin to Vpeak
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @retval DeltaT - Calculating time value from Vmin to Vpeak
  *
  */
double BulkCap::DeltaT(double VInMin, double VRectMinPeak, int FLine)
{
    return (asin(VInMin/VRectMinPeak))/(2*PI*FLine);
}

/**
  * @brief The total charging time
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @retval ChargTime - total charging time value
  */
double BulkCap::ChargTime(double VInMin, double VRectMinPeak, int FLine)
{
    return ((1/(4*FLine))-(BulkCap::DeltaT(VInMin, VRectMinPeak, FLine)));
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
double BulkCap::CapValue(double VInMin, double VRectMinPeak, int FLine, double POut, double Eff)
{
    return ((2*POut)*(1/(4*FLine))+(DeltaT(VInMin, VRectMinPeak, FLine)))/(Eff*((VRectMinPeak*VRectMinPeak)-(VInMin*VInMin)));
}

/**
  * @brief Load peak current value
  * @param POut - summary output power of the converter
  * @param Eff - efficiency
  * @param VInMinRMS - Minimum RMS value line voltage
  * @retval ILoadMax - peak current value
  */
double BulkCap::ILoadMax(double POut, double Eff, int VInMinRMS)
{
    return POut/(Eff*VInMinRMS);
}

/**
  * @brief Load minimum current value
  * @param POut - summary output power of the converter
  * @param Eff - efficiency
  * @param VInMaxRMS - Maximum RMS value line voltage
  * @retval ILoadMin - minimum current value
  */
double BulkCap::ILoadMin(double POut, double Eff, int VInMaxRMS)
{
    return POut/(Eff*VInMaxRMS);
}

/**
  * @brief The bulk capacitor peak current
  * @param CapVal - bulk capacitor value
  * @param VRectMinPeak - peak voltage value after diode blidge and capacitor
  * @param VInMin - minimum voltage value after diode blidge and capacitor
  * @param FLine - frequency in power line
  * @retval IBulkCapPeak - bulk capacitor peak current
  */
double BulkCap::IBulkCapPeak(double CapVal, double VRectMinPeak, double VInMin, int FLine)
{
    return (2*PI*FLine*CapVal*VRectMinPeak*(cos(2*PI*FLine*DeltaT(VInMin, VRectMinPeak, FLine))));
}

/**
  * @brief Calculate bulk capacitor RMS current value
  * @param ILoadAVG - diode average current
  * @param DiodeConductTime - total conduction time for diode
  * @param FLine - frequency in power line
  * @retval IBulkCapRMS - bulk capacitor RMS current
  */
double BulkCap::IBulkCapRMS(double ILoadAVG, double DiodeConductTime, int FLine)
{
    return ILoadAVG*(sqrt((2/(3*FLine*DiodeConductTime))-1));
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::VMinInp()
{
    return 2*cos(PI);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::VDCMin()
{
    return 2*cos(PI);
}
