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
  * @brief
  * @param
  * @retval
  */
double BulkCap::DeltaT(double VInMin, double VRectMinPeak, int FLine)
{
    return (asin(VInMin/VRectMinPeak))/(2*PI*FLine);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::ChargTime()
{
    return 2*cos(PI);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::CapValue()
{
    return 2*cos(PI);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::ILoadMax()
{
    return 2*cos(PI);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::ILoadMin()
{
    return 2*cos(PI);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::IBulkCapPeak()
{
    return 2*cos(PI);
}

/**
  * @brief
  * @param
  * @retval
  */
double BulkCap::IBulkCapRMS()
{
    return 2*cos(PI);
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
