/**
  * License text...
*/
#ifndef BULKCAP_H
#define BULKCAP_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class BulkCap
{
public:
    /**
     * @brief BulkCap -
     * @param invmax - Max input AC line voltage V
     * @param invmin - Min input AC line voltage V
     * @param ef - Efficiency ratio
     * @param pout - Total max power output W
     * @param fl - Line frequency default 50Hz
     */
    BulkCap(int16_t invmax, int16_t invmin,
            float ef, float pout,
            float fl = 50.0f):
    acinvmax(invmax), acinvmin(invmin),
    eff(ef), pmaxout(pout),
    frline(fl)
    {}

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
    int16_t acinvmax;
    int16_t acinvmin;
    float eff;
    float pmaxout;
    float frline;

    inline double VDOut() const;
    inline double VRRMS() const;

};

#endif // BULKCAP_H
