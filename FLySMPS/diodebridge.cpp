/**
  * License text...
*/
#include "diodebridge.h"

/**
 * @brief IDiodePeak - Calculate the diode peak current
 * @return diode peak current
 */
double DiodeBridge::IDiodePeak() const
{
    return static_cast<double>(bcappc)+curmaxl;
}

/**
 * @brief DiodeCurrentSlope - Diode current down slope from the peak value to total charging time
 * @return Diode current down slope, in A/s
 */
double DiodeBridge::DiodeCurrentSlope() const
{
    return (IDiodePeak()-curminl)/chrgtm;
}

/**
 * @brief DiodeConductTime - The diode total conduction time
 * @return total conduction time for diode
 */
double DiodeBridge::DiodeConductTime() const
{
    return IDiodePeak()/DiodeCurrentSlope();
}

/**
 * @brief ILoadAVG - Calculate the average dc current
 * @param  frline - frequency in power line
 * @return average dc current
 */
double DiodeBridge::ILoadAVG() const
{
    return IDiodePeak()*static_cast<double>(frline)*DiodeConductTime();
}

/**
 * @brief IDiodeAVG - Calculate diode average current
 * @return average current value
 */
double DiodeBridge::IDiodeAVG() const
{
    return ILoadAVG()/2.;
}

/**
 * @brief IDiodeRMS - Root mean square value of current diode
 * @return rms current diode
 */
double DiodeBridge::IDiodeRMS() const
{
    return ILoadAVG()/(std::sqrt(3.* static_cast<double>(frline)*DiodeConductTime()));
}

/**
 * @brief IDiodeRMSTot - Total root mean square value of current diode
 * @return total rms current diode
 */
double DiodeBridge::IDiodeRMSTot() const
{
    return (ILoadAVG()*std::sqrt(2.))/(std::sqrt(3.*static_cast<double>(frline)*DiodeConductTime()));
}

/**
 * @brief MinPeakInVoltage - Solve minimum peak value
 * @return minimum peak value
 */
double DiodeBridge::MinPeakInVoltage() const
{
    return acinvmin*std::sqrt(2.);
}

/**
 * @brief MaxPeakInVoltage - Solve maximum peak value
 * @return maximum peak value
 */
double DiodeBridge::MaxPeakInVoltage() const
{
    return  acinvmax*std::sqrt(2.);
}
