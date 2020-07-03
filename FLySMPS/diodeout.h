#ifndef DIODEOUT_H
#define DIODEOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class DiodeOut
{
public:
    /**
      * @brief set pre- calculation value
      * @param ps secondary output power
      * @param vo output voltage
      * @param tr turns ratio
      * @return
      */
    DiodeOut(double ps, double vo, double tr): power_sec(ps), volt_out(vo), turn_ratio(tr){}
    double doDiodeRevVolt(const DBridge& dbvalue);
    double doDiodePowLoss(const InputValue& ivalue);
private:
    double power_sec;
    double volt_out;
    double turn_ratio;
};

#endif // DIODEOUT_H