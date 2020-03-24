////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#include "diodebridge.h"
//#define PI 3.1415926535897932385;

//Constructor
DiodeBridge::DiodeBridge()
{

}

/**
  * @brief  Calculate the diode peak current
  * @param  ICBulckPeak - bulk capacitor peak current
  * @param  ILoadMax - load maximum current
  * @retval IDiodePeak - diode peak current
  */
double DiodeBridge::IDiodePeak(double ICBulckPeak, double ILoadMax)
{
    return ICBulckPeak+ILoadMax;
}

/**
  * @brief  Root mean square value of current diode
  * @param  ILoadAVG - diode average current
  * @param  FLine - frequency in power line
  * @param  ConductTime - diode total conduction time
  * @retval IDiodeRMS - rms current diode
  */
double DiodeBridge::IDiodeRMS(double ILoadAVG, double FLine, double ConductTime)
{
    return ILoadAVG/(sqrt(3*FLine*ConductTime));
}

/**
  * @brief  Calculate diode average current
  * @param  ILoadAVG - average dc current
  * @retval IDiodeAVG - average current value
  */
double DiodeBridge::IDiodeAVG(double ILoadAVG)
{
    return ILoadAVG/2;
}

/**
  * @brief  Total root mean square value of current diode
  * @param  ILoadAVG - diode average current
  * @param  FLine - frequency in power line
  * @param  ConductTime - diode total conduction time
  * @retval IDiodeRMSTot - total rms current diode
  */
double DiodeBridge::IDiodeRMSTot(double ILoadAVG, double FLine, double ConductTime)
{
    return (ILoadAVG*sqrt(2))/(sqrt(3*FLine*ConductTime));
}

/**
  * @brief  Calculate the average dc current
  * @param  IDiodePeak - diode peak current
  * @param  FLine - frequency in power line
  * @param  ConductTime - diode total conduction time
  * @retval ILoadAVG - average dc current
  */
double DiodeBridge::ILoadAVG(double IDiodePeak, double FLine, double ConductTime)
{
    return IDiodePeak*FLine*ConductTime;
}

/**
  * @brief  The diode total conduction time
  * @param  IDiodePeak - diode peak current
  * @param  DiodeCurrentSlope - diode current down slope
  * @retval DiodeConductTime - total conduction time for diode
  */
double DiodeBridge::DiodeConductTime(double IDiodePeak, double DiodeCurrentSlope)
{
    return IDiodePeak/DiodeCurrentSlope;
}

/**
  * @brief  Diode current down slope from the peak value to total charging time
  * @param  IDiodePeak - diode peak current
  * @param  ILoad - load peak current value
  * @param  TotCharTime - total charging time
  * @retval DiodeCurrentSlope - Diode current down slope, in A/s
  */
double DiodeBridge::DiodeCurrentSlope(double IDiodePeak, double ILoad, double TotCharTime)
{
    return (IDiodePeak-ILoad)/TotCharTime;
}

/**
  * @brief  Solve minimum peak value
  * @param  VACmin - minimum ac line amplitude value
  * @retval MinPeakInVoltage - minimum peak value
  */
double DiodeBridge::MinPeakInVoltage(int VACmin)
{
    return VACmin*sqrt(2);
}

/**
  * @brief  Solve maximum peak value
  * @param  VACmax - maximum ac line amplitude value
  * @retval MaxPeakInVoltage - maximum peak value
  */
double DiodeBridge::MaxPeakInVoltage(int VACmax)
{
    return VACmax*sqrt(2);
}
