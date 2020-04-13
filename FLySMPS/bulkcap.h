////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#ifndef BULKCAP_H
#define BULKCAP_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class BulkCap
{
public:
    BulkCap();

    double DeltaT(InputValue *ivalue);
    double ChargTime(InputValue *ivalue);

    double CapValue(double VInMin, double VRectMinPeak, int8_t FLine, double POut, double Eff);

    double ILoadMax(InputValue* ivalue, DBridge* dbvalue);
    double ILoadMin(InputValue* ivalue, DBridge* dbvalue);
    double IBulkCapPeak(double CapVal, double VRectMinPeak, double VInMin, int8_t FLine);
    double IBulkCapRMS(double ILoadAVG, double DiodeConductTime, int8_t FLine);

    double VMinInp(double VInMin, double VRectMinPeak, double POut, double CapVal, int8_t FLine);
    double VDCMin(double VRectMinPeak, double VMinInp);

};

#endif // BULKCAP_H
