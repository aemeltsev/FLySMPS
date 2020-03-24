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
double DiodeBridge::IDiodeAVG()
{
    return PI+PI;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::IDiodeRMSTot()
{
    return PI+PI;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::ILoadAVG()
{
    return PI+PI;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::DiodeConductTime()
{
    return PI+PI;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::DiodeCurrentSlope()
{
    return PI+PI;
}

/**
  * @brief  None
  * @param  None
  * @retval None
  */
double DiodeBridge::MinPeakInVoltage()
{
    return PI+PI;
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
