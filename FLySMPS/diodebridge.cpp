#include "diodebridge.h"
#define PI 3.1415926535897932385;

DiodeBridge::DiodeBridge()
{

}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::IDiodePeak(double ICBulckPeak, double ILoadMax)
{
    return ICBulckPeak+ILoadMax;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::IDiodeRMS(double ILoadAVG, double FLine, double ConductTime)
{
    return ILoadAVG/(sqrt(3*FLine*ConductTime));
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::IDiodeAVG(double ILoadAVG)
{
    return ILoadAVG/2;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::IDiodeRMSTot(double ILoadAVG, double FLine, double ConductTime)
{
    return (ILoadAVG*sqrt(2))/(sqrt(3*FLine*ConductTime));
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::ILoadAVG(double IDiodePeak, double FLine, double ConductTime)
{
    return IDiodePeak*FLine*ConductTime;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::DiodeConductTime(double IDiodePeak, double DiodeCurrentSlope)
{
    return IDiodePeak/DiodeCurrentSlope;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::DiodeCurrentSlope(double IDiodePeak, double ILoad, double TotCharTime)
{
    return (IDiodePeak-ILoad)/TotCharTime;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::MinPeakInVoltage(int VACmin)
{
    return (double)VACmin*sqrt(2);
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::MaxPeakInVoltage()
{
    return PI+PI;
}
