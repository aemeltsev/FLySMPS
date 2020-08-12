#include "diodeout.h"

/**
 * @brief doDiodeRevVolt
 * @param ac_inp_volt_max - input max AC line voltage
 * @return max reverse voltage
 */
double DiodeOut::doDiodeRevVolt(int16_t ac_inp_volt_max) const
{
    return (volt_out+(ac_inp_volt_max/(std::sqrt(2))))/turn_ratio;
}

/**
 * @brief doDiodePowLoss - losses on the diode
 * @param dio_drop - volt drop on schottky diode
 * @return losses value
 */
double DiodeOut::doDiodePowLoss(float dio_drop) const
{
    return (power_sec* static_cast<double>(dio_drop))/volt_out;
}
