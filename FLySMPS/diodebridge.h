////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
//#include <stdint.h>
#include <math.h>

class DiodeBridge
{
public:
    DiodeBridge();

    double IDiodePeak(double ICBulckPeak, double ILoadMax);
    double IDiodeRMS(double ILoadAVG, int FLine, double ConductTime);
    double IDiodeAVG(double ILoadAVG);

    double IDiodeRMSTot(double ILoadAVG, int FLine, double ConductTime);

    double ILoadAVG(double IDiodePeak, int FLine, double ConductTime);
    
    double DiodeCurrentSlope(double IDiodePeak, double ILoad, double TotCharTime);
    double DiodeConductTime(double IDiodePeak, double DiodeCurrentSlope);

    double MinPeakInVoltage(int VACmin);
    double MaxPeakInVoltage(int VACmax);
};

#endif // DIODEBRIDGE_H
