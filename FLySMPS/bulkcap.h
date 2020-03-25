////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#ifndef BULKCAP_H
#define BULKCAP_H
#include <math.h>


class BulkCap
{
public:
    BulkCap();

    double DeltaT(double VInMin, double VRectMinPeak, int FLine);
    double ChargTime();

    double CapValue();

    double ILoadMax();
    double ILoadMin();
    double IBulkCapPeak();
    double IBulkCapRMS();

    double VMinInp();
    double VDCMin();

};

#endif // BULKCAP_H
