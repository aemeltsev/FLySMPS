#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
#include <stdint.h>
#include <math.h>

class DiodeBridge
{
public:
    DiodeBridge();

    double IDiodePeak();
    double IDiodeRMS();
    double IDiodeAVG();

    double IDiodeRMSTot();

    double ILoadAVG();

    double DiodeConductTime();
    double DiodeCurrentSlope();

    double MinPeakInVoltage();
    double MaxPeakInVoltage();
};

#endif // DIODEBRIDGE_H
