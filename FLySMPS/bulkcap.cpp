/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include "bulkcap.h"

/**
 * @brief DeltaT - The total refueling time from Vmin to Vpeak
 * @return Calculating time value from Vmin to Vpeak
 */
double BulkCap::DeltaT() const
{
    return (qAsin(VDOut()/(ac_inp_volt_min*qSqrt(2))))/(2.*M_PI*(static_cast<double>(freq_line)));
}

/**
 * @brief ChargTime - The total charging time
 * @return total charging time value
 */
double BulkCap::ChargTime() const
{ 
    return ((1./(4.*static_cast<double>(freq_line)))-(DeltaT()));
}

/**
 * @brief CapValue - Calculate the bulk capacitor value
 * @return bulk capacitor value
 */
double BulkCap::CapValue() const
{
    return ((2.*static_cast<double>(pow_max_out))*(1./(4.*static_cast<double>(freq_line)))+DeltaT())/(static_cast<double>(efficiency)*(qPow(VRRMS(),2))-(qPow(VDOut(),2)));
}

/**
 * @brief ILoadMax - Load peak current value
 * @return peak current value
 */
double BulkCap::ILoadMax() const
{
    return (static_cast<double>(pow_max_out))/(static_cast<double>(efficiency)*(ac_inp_volt_min/qSqrt(2)));
}

/**
 * @brief ILoadMin - Load minimum current value
 * @return minimum current value
 */
double BulkCap::ILoadMin() const
{
    return (static_cast<double>(pow_max_out))/(static_cast<double>(efficiency)*(ac_inp_volt_max/qSqrt(2)));
}

/**
 * @brief IBulkCapPeak - The bulk capacitor peak current
 * @return bulk capacitor peak current A
 */
double BulkCap::IBulkCapPeak() const
{
    return 2. * M_PI * static_cast<double>(freq_line) * CapValue() * VDOut() * (qCos(2. * M_PI * static_cast<double>(freq_line) * DeltaT()));
}

/**
 * @brief IBulkCapRMS - Calculate bulk capacitor RMS current value
 * @param dio_av_curr - diode average current
 * @param dio_cond_time - total conduction time for diode
 * @return bulk capacitor RMS current
 */
double BulkCap::IBulkCapRMS(double dio_av_curr, double dio_cond_time) const
{
    return dio_av_curr*(qSqrt((2./(3.*static_cast<double>(freq_line)*dio_cond_time))-1));
}

/**
 * @brief VMinInp - Recalculation after input capacitor selection
 * @return recalculation after input capacitor selection
 */
double BulkCap::VMinInp() const
{
    return qSqrt(qPow(VDOut(),2)-((2.*static_cast<double>(pow_max_out)*((1./(4.*static_cast<double>(freq_line))-DeltaT())))/CapValue()));
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
    return (ac_inp_volt_min*(1./(qSqrt(2))))-(ac_inp_volt_min*(2./M_PI));
}

/**
 * @brief VDOut
 * @return
 */
double BulkCap::VDOut() const
{
    return (ac_inp_volt_min*qSqrt(2.))-VRRMS();
}
