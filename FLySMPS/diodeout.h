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
