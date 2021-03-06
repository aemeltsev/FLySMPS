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

#ifndef DIODEOUT_H
#define DIODEOUT_H
#include <QtMath>
#include <cstdint>

class DiodeOut
{
public:

    /**
     * @brief DiodeOut - set pre- calculation value
     * @param ps - secondary output power
     * @param vo - output voltage
     * @param tr - turns ratio
     */
    DiodeOut(double ps, double vo, double tr)
        :power_sec(ps)
        ,volt_out(vo)
        ,turn_ratio(tr)
    {}

    /**
     * @brief doDiodeRevVolt
     * @param ac_inp_volt_max - input max AC line voltage
     * @return max reverse voltage
     */
    double doDiodeRevVolt(int16_t ac_inp_volt_max) const
    {
        return (volt_out+(ac_inp_volt_max/(qSqrt(2))))/turn_ratio;
    }

    /**
     * @brief doDiodePowLoss - losses on the diode
     * @param dio_drop - volt drop on schottky diode
     * @return losses value
     */
    double doDiodePowLoss(float dio_drop) const
    {
        return (power_sec* static_cast<double>(dio_drop))/volt_out;
    }

private:
    double power_sec;
    double volt_out;
    double turn_ratio;
};
#endif // DIODEOUT_H
