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

/**
 * @brief The DiodeOut class
 *        See more:
 *        TIDA-010000.-50W opto-regulated multi-output flyback design for 380-480VAC motor drives
 *        TIDUBK6A.-Multiple isolated output, auxiliary supply reference design
 *        AN5504.-Designing Flyback Converters Using Peak-Current-Mode Controllers
 */
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
     * @return max reverse voltage V
     */
    double doDiodeRevVolt(int16_t ac_inp_volt_max) const
    {
        return volt_out + (ac_inp_volt_max / turn_ratio);
    }

    /**
     * @brief doDiodePowLoss - losses on the diode
     * @param dio_drop - volt drop on schottky diode
     * @return losses value W
     */
    double doDiodePowLoss(float dio_drop) const
    {
        return (power_sec * static_cast<double>(dio_drop)) / volt_out;
    }

    /**
     * @brief doDiodeCurrPeak - maximum secondary peak current
     * @param cur_pri_pk - maximum primary peak current
     * @return output current value in A
     */
    double doDiodeCurrPeak(float cur_pri_pk)
    {
        return static_cast<double>(cur_pri_pk) / turn_ratio;
    }

    /**
     * @brief doDiodeCurrRMS - maximum secondary RMS current
     * @param cur_pri_pk - maximum primary peak current
     * @return output current value in A
     */
    double doDiodeCurrRMS(float cur_pri_pk)
    {
        double cur_sec = power_sec / volt_out;
        return qSqrt((2. * cur_sec * static_cast<double>(cur_pri_pk)) / (3. * turn_ratio));
    }

private:
    double power_sec;
    double volt_out;
    double turn_ratio;
};
#endif // DIODEOUT_H
