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

#include "diodebridge.h"

/**
 * @brief IDiodePeak - Calculate the diode peak current
 * @return diode peak current
 */
double DiodeBridge::IDiodePeak() const
{
    return static_cast<double>(cap_peak_curr)+cur_max_load;
}

/**
 * @brief DiodeCurrentSlope - Diode current down slope from the peak value to total charging time
 * @return Diode current down slope, in A/s
 */
double DiodeBridge::DiodeCurrentSlope() const
{
    return (IDiodePeak()-cur_min_load)/cap_char_time;
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
    return IDiodePeak()*static_cast<double>(freq_line)*DiodeConductTime();
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
    return ILoadAVG()/(qSqrt(3.* static_cast<double>(freq_line)*DiodeConductTime()));
}

/**
 * @brief IDiodeRMSTot - Total root mean square value of current diode
 * @return total rms current diode
 */
double DiodeBridge::IDiodeRMSTot() const
{
    return (ILoadAVG()*qSqrt(2.))/(qSqrt(3.*static_cast<double>(freq_line)*DiodeConductTime()));
}

/**
 * @brief MinPeakInVoltage - Solve minimum peak value
 * @return minimum peak value
 */
double DiodeBridge::MinPeakInVoltage() const
{
    return ac_inp_volt_min*qSqrt(2.);
}

/**
 * @brief MaxPeakInVoltage - Solve maximum peak value
 * @return maximum peak value
 */
double DiodeBridge::MaxPeakInVoltage() const
{
    return  ac_inp_volt_max*qSqrt(2.);
}
