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
    double ChargTime(double VInMin, double VRectMinPeak, int FLine);

    double CapValue(double VInMin, double VRectMinPeak, int FLine, double POut, double Eff);

    double ILoadMax(double POut, double Eff, int VInMinRMS);
    double ILoadMin(double POut, double Eff, int VInMaxRMS);
    double IBulkCapPeak(double CapVal, double VRectMinPeak, double VInMin, int FLine);
    double IBulkCapRMS(double ILoadAVG, double DiodeConductTime, int FLine);

    double VMinInp(double VInMin, double VRectMinPeak, double POut, double CapVal, int FLine);
    double VDCMin(double VRectMinPeak, double VMinInp);

};

#endif // BULKCAP_H
