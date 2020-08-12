/**
  * License text...
*/
#include "bulkcap.h"

/**
 * @brief DeltaT - The total refueling time from Vmin to Vpeak
 * @return Calculating time value from Vmin to Vpeak
 */
double BulkCap::DeltaT() const
{
    return (std::asin(VDOut()/(acinvmin*std::sqrt(2))))/(2.*S_PI*(static_cast<double>(frline)));
}

/**
 * @brief ChargTime - The total charging time
 * @return total charging time value
 */
double BulkCap::ChargTime() const
{ 
    return ((1./(4.*static_cast<double>(frline)))-(DeltaT()));
}

/**
 * @brief CapValue - Calculate the bulk capacitor value
 * @return bulk capacitor value
 */
double BulkCap::CapValue() const
{
    return ((2.*static_cast<double>(pmaxout))*(1./(4.*static_cast<double>(frline)))+DeltaT())/(static_cast<double>(eff)*(std::pow(VRRMS(),2))-(std::pow(VDOut(),2)));
}

/**
 * @brief ILoadMax - Load peak current value
 * @return peak current value
 */
double BulkCap::ILoadMax() const
{
    return (static_cast<double>(pmaxout))/(static_cast<double>(eff)*(acinvmin/std::sqrt(2)));
}

/**
 * @brief ILoadMin - Load minimum current value
 * @return minimum current value
 */
double BulkCap::ILoadMin() const
{
    return (static_cast<double>(pmaxout))/(static_cast<double>(eff)*(acinvmax/std::sqrt(2)));
}

/**
 * @brief IBulkCapPeak - The bulk capacitor peak current
 * @return bulk capacitor peak current A
 */
double BulkCap::IBulkCapPeak() const
{
    return 2. * S_PI * static_cast<double>(frline) * CapValue() * VDOut() * (std::cos(2. * S_PI * static_cast<double>(frline) * DeltaT()));
}

/**
 * @brief IBulkCapRMS - Calculate bulk capacitor RMS current value
 * @param lavgc - diode average current
 * @param dct - total conduction time for diode
 * @return bulk capacitor RMS current
 */
double BulkCap::IBulkCapRMS(double lavgc, double dct) const
{
    return lavgc*(std::sqrt((2./(3.*static_cast<double>(frline)*dct))-1));
}

/**
 * @brief VMinInp - Recalculation after input capacitor selection
 * @return recalculation after input capacitor selection
 */
double BulkCap::VMinInp() const
{
    return std::sqrt(std::pow(VDOut(),2)-((2.*static_cast<double>(pmaxout)*((1./(4.*static_cast<double>(frline))-DeltaT())))/CapValue()));
}

/**
 * @brief VDCMin - simply the average value of MinInp and VRectMinPeak
 * @return simply the average value of MinIng and VRectMinPeak
 */
double BulkCap::VDCMin() const
{
    return (VDOut()+CapValue())/2.;
}

/**
 * @brief VRRMS
 * @return
 */
double BulkCap::VRRMS() const
{
    return (acinvmin*(1./(std::sqrt(2))))-(acinvmin*(2./S_PI));
}

/**
 * @brief VDOut
 * @return
 */
double BulkCap::VDOut() const
{
    return (acinvmin*std::sqrt(2.))-VRRMS();
}
