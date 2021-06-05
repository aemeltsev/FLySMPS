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
    DiodeOut(double ps, double vo, double tr):
        power_sec(ps), volt_out(vo),
        turn_ratio(tr)
    {}
    inline double doDiodeRevVolt(int16_t ac_inp_volt_max) const;
    inline double doDiodePowLoss(float dio_drop) const;
private:
    double power_sec;
    double volt_out;
    double turn_ratio;
};

#endif // DIODEOUT_H
