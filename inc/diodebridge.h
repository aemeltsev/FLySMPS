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

#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
#include <QtMath>
#include <cstdint>

class DiodeBridge
{
public:
    /**
     * @brief DiodeBridge - Input rectifier diode bridge
     * @param max_volt - Max input AC line voltage V
     * @param min_volt - Min input AC line voltage V
     * @param eff - Efficiency ratio
     * @param pout - Total max power output W
     * @param fl - Line frequency default 50Hz
     */
    DiodeBridge(int16_t max_volt, int16_t min_volt,
                float eff, float pout,
                float fl = 50.0f)
        :ac_inp_volt_max(max_volt)
        ,ac_inp_volt_min(min_volt)
        //,efficiency(eff)
        //,pow_max_out(pout)
        ,freq_line(fl)
        ,cur_max_load(static_cast<double>(pout) / (eff * min_volt))
        ,cur_min_load(static_cast<double>(pout) / (eff * max_volt))
    {}

    /**
     * @brief setBcapParam
     * @param bcpc - bulk capacitor peak current
     * @param ct - the capacitor total charging time
     */
    void setBcapParam(float bcpc, double ct)
    {
        cap_peak_curr = bcpc;
        cap_char_time = ct;
    }

    /**
     * @brief IDiodePeak - Calculate the diode peak current
     * @return diode peak current
     */
    inline double IDiodePeak() const
    {
        return static_cast<double>(cap_peak_curr)+cur_max_load;
    }

    /**
     * @brief DiodeCurrentSlope - Diode current down slope from the peak value to total charging time
     * @return Diode current down slope, in A/s
     */
    inline double DiodeCurrentSlope() const
    {
        return (IDiodePeak()-cur_min_load)/cap_char_time;
    }

    /**
     * @brief DiodeConductTime - The diode total conduction time
     * @return total conduction time for diode
     */
    double DiodeConductTime() const
    {
        return IDiodePeak()/DiodeCurrentSlope();
    }

    /**
     * @brief ILoadAVG - Calculate the average dc current
     * @param  frline - frequency in power line
     * @return average dc current
     */
    double ILoadAVG() const
    {
        return IDiodePeak()*static_cast<double>(freq_line)*DiodeConductTime();
    }

    /**
     * @brief IDiodeAVG - Calculate diode average current
     * @return average current value
     */
    double IDiodeAVG() const
    {
        return ILoadAVG()/2.;
    }

    /**
     * @brief IDiodeRMS - Root mean square value of current diode
     * @return rms current diode
     */
    double IDiodeRMS() const
    {
        return ILoadAVG()/(qSqrt(3.* static_cast<double>(freq_line)*DiodeConductTime()));
    }

    /**
     * @brief IDiodeRMSTot - Total root mean square value of current diode
     * @return total rms current diode
     */
    double IDiodeRMSTot() const
    {
        return (ILoadAVG() * M_SQRT2)/(qSqrt(3.*static_cast<double>(freq_line)*DiodeConductTime()));
    }

    /**
     * @brief MinPeakInVoltage - Solve minimum peak value
     * @return minimum peak value
     */
    double MinPeakInVoltage() const
    {
        return ac_inp_volt_min * M_SQRT2;
    }

    /**
     * @brief MaxPeakInVoltage - Solve maximum peak value
     * @return maximum peak value
     */
    double MaxPeakInVoltage() const
    {
        return  ac_inp_volt_max * M_SQRT2;
    }

private:
    int16_t ac_inp_volt_max;
    int16_t ac_inp_volt_min;
    //float efficiency;
    //float pow_max_out;
    float freq_line;

    double cap_char_time = 0.0; // the capacitor total charging time
    float cap_peak_curr = 0.0f; // bulk capacitor peak current

    double cur_max_load; // load maximum current
    double cur_min_load; // load minimum current

};
#endif // DIODEBRIDGE_H
