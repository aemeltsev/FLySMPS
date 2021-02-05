/**
  * License text...
*/
#ifndef BULKCAP_H
#define BULKCAP_H
#include <QtMath>
#include <cstdint>

class BulkCap
{
public:
    /**
     * @brief BulkCap -
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
