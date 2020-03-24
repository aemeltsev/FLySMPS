#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
#include <stdint.h>
#include <math.h>

class DiodeBridge
{
public:
    DiodeBridge();

    double IDiodePeak(double ICBulckPeak, double ILoadMax);
    double IDiodeRMS(double ILoadAVG, double FLine, double ConductTime);
    double IDiodeAVG(double ILoadAVG);

    double IDiodeRMSTot(double ILoadAVG, double FLine, double ConductTime);

    double ILoadAVG();

    double DiodeConductTime();
    double DiodeCurrentSlope();

    double MinPeakInVoltage();
    double MaxPeakInVoltage();
};

#endif // DIODEBRIDGE_H
