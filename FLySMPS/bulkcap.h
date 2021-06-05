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
            float fl = 50.0f):
    ac_inp_volt_max(max_volt), ac_inp_volt_min(min_volt),
    efficiency(eff), pow_max_out(pout),
    freq_line(fl)
    {}
    ~BulkCap(){}

    inline double DeltaT() const;
    inline double ChargTime() const;
    inline double CapValue() const;
    inline double ILoadMax() const;
    inline double ILoadMin() const;
    inline double IBulkCapPeak() const;
    inline double IBulkCapRMS(double lavgc, double dct) const;
    inline double VMinInp() const;
    inline double VDCMin() const;

private:
    int16_t ac_inp_volt_max;
    int16_t ac_inp_volt_min;
    float efficiency;
    float pow_max_out;
    float freq_line;

    inline double VDOut() const;
    inline double VRRMS() const;

};

#endif // BULKCAP_H
