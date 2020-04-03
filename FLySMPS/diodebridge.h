////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
#include <cmath>
#include <cstdint>

class DiodeBridge
{
public:
    DiodeBridge();

    double IDiodePeak(double ICBulckPeak, double ILoadMax);
    double IDiodeRMS(double ILoadAVG, int8_t FLine, double ConductTime);
    double IDiodeAVG(double ILoadAVG);

    double IDiodeRMSTot(double ILoadAVG, int8_t FLine, double ConductTime);

    double ILoadAVG(double IDiodePeak, int8_t FLine, double ConductTime);
    
    double DiodeCurrentSlope(double IDiodePeak, double ILoad, double TotCharTime);
    double DiodeConductTime(double IDiodePeak, double DiodeCurrentSlope);

    double MinPeakInVoltage(int8_t VACmin);
    double MaxPeakInVoltage(int8_t VACmax);
};

#endif // DIODEBRIDGE_H
