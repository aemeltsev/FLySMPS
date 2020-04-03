////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#ifndef BULKCAP_H
#define BULKCAP_H
#include <cmath>
#include <cstdint>

class BulkCap
{
public:
    BulkCap();

    double DeltaT(double VInMin, double VRectMinPeak, int8_t FLine);
    double ChargTime(double VInMin, double VRectMinPeak, int8_t FLine);

    double CapValue(double VInMin, double VRectMinPeak, int8_t FLine, double POut, double Eff);

    double ILoadMax(double POut, double Eff, int8_t VInMinRMS);
    double ILoadMin(double POut, double Eff, int8_t VInMaxRMS);
    double IBulkCapPeak(double CapVal, double VRectMinPeak, double VInMin, int8_t FLine);
    double IBulkCapRMS(double ILoadAVG, double DiodeConductTime, int8_t FLine);

    double VMinInp(double VInMin, double VRectMinPeak, double POut, double CapVal, int8_t FLine);
    double VDCMin(double VRectMinPeak, double VMinInp);

};

#endif // BULKCAP_H
