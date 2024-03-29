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

#ifndef BULKCAP_H
#define BULKCAP_H
#include <QtMath>
#include <cstdint>

class BulkCap
{
private:
    int16_t ac_inp_volt_max;
    int16_t ac_inp_volt_min;
    float efficiency;
    float pow_max_out;
    float freq_line;
    int16_t v_dc_in_rippl = 30;

public:
    /**
     * @brief BulkCap - Input smoothing capacitor
     * @param max_volt - Max input AC line voltage V
     * @param min_volt - Min input AC line voltage V
     * @param eff - Efficiency ratio
     * @param pout - Total max power output W
     * @param fl - Line frequency default 50Hz
     */
    BulkCap(int16_t max_volt, int16_t min_volt,
            float eff, float pout,
            float fl = 50.0f)
        :ac_inp_volt_max(max_volt)
        ,ac_inp_volt_min(min_volt)
        ,efficiency(eff)
        ,pow_max_out(pout)
        ,freq_line(fl)
    {}

    /**
     * @brief DeltaT - The total refueling time from Vmin to Vpeak
     * @return Calculating time value from Vmin to Vpeak
     */
    double DeltaT() const
    {
        double num = qAsin(ac_inp_volt_min / (ac_inp_volt_min * M_SQRT2));
        double dnm = 2.0 * M_PI * (static_cast<double>(freq_line));
        return num/dnm;
    }

    /**
     * @brief ChargTime - The total charging time
     * @return total charging time value
     */
    double ChargTime() const
    {
        double frq_coeff = 1.0 / (4.0 * static_cast<double>(freq_line));
        return frq_coeff - DeltaT();
    }

    /**
     * @brief CapValue - Calculate the bulk capacitor value
     * @return bulk capacitor value
     */
    double CapValue() const
    {
        double pwr_coeff = 2.0 * static_cast<double>(pow_max_out);
        double frq_coeff = 1.0 / (4.0 * static_cast<double>(freq_line));
        double num = pwr_coeff * (frq_coeff + DeltaT());
        double v_dc_min = ac_inp_volt_min * M_SQRT2 * 0.75;
        double v_min_pre = v_dc_min - v_dc_in_rippl;
        double dnm = static_cast<double>(efficiency) * (qPow(v_dc_min, 2) - qPow(v_min_pre, 2));
        return num / dnm;
     }

    /**
     * @brief ILoadMax - Load peak current value
     * @return peak current value
     */
    double ILoadMax() const
    {
        return (static_cast<double>(pow_max_out))/(static_cast<double>(efficiency)*(ac_inp_volt_min/qSqrt(2)));
    }

    /**
     * @brief ILoadMin - Load minimum current value
     * @return minimum current value
     */
    double ILoadMin() const
    {
        return (static_cast<double>(pow_max_out))/(static_cast<double>(efficiency)*(ac_inp_volt_max/qSqrt(2)));
    }

    /**
     * @brief IBulkCapPeak - The bulk capacitor peak current
     * @return bulk capacitor peak current A
     */
    double IBulkCapPeak() const
    {
        return 2. * M_PI * static_cast<double>(freq_line) * CapValue() * (ac_inp_volt_min * M_SQRT2) * (qCos(2. * M_PI * static_cast<double>(freq_line) * DeltaT()));
    }

    /**
     * @brief IBulkCapRMS - Calculate bulk capacitor RMS current value
     * @param dio_av_curr - diode average current
     * @param dio_cond_time - total conduction time for diode
     * @return bulk capacitor RMS current
     */
    double IBulkCapRMS(double dio_av_curr, double dio_cond_time) const
    {
        return dio_av_curr*(qSqrt((2./(3.*static_cast<double>(freq_line)*dio_cond_time))-1));
    }

    /**
     * @brief VMinInp - Recalculation after input capacitor selection
     * @return recalculation after input capacitor selection
     */
    double VMinInp() const
    {
        return qSqrt(qPow((ac_inp_volt_min * M_SQRT2),2)-((2.*static_cast<double>(pow_max_out)*((1./(4.*static_cast<double>(freq_line))-DeltaT())))/CapValue()));
    }

    /**
     * @brief VDCMin - simply the average value of MinInp and VRectMinPeak
     * @return simply the average value of MinIng and VRectMinPeak
     */
    double VDCMin() const
    {
        return 0.5 * ((ac_inp_volt_min * M_SQRT2) + VMinInp());
    }
};
#endif // BULKCAP_H
