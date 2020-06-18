#include "diodeout.h"

double DiodeOut::doDiodeRevVolt(const DBridge& dbvalue)
{
    return (volt_out+dbvalue.in_max_rms_voltage)/turn_ratio;
}

double DiodeOut::doDiodePowLoss(const InputValue& ivalue)
{
    return (power_sec*ivalue.volt_diode_drop_sec)/volt_out;
}
