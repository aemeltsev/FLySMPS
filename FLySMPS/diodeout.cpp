#include "diodeout.h"
/**
  * @brief calc diode reverse voltage
  * @param dbvalue input max rms voltage
  * @return max reverse voltage
  */
double DiodeOut::doDiodeRevVolt(const DBridge& dbvalue)
{
    return (volt_out+dbvalue.in_max_rms_voltage)/turn_ratio;
}
/**
  * @brief losses on the diode
  * @param ivalue volt drop on schottky diode
  * @return losses value
  */
double DiodeOut::doDiodePowLoss(const InputValue& ivalue)
{
    return (power_sec*ivalue.volt_diode_drop_sec)/volt_out;
}
