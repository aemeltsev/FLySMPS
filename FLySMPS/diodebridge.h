////////////////////////////////////////////////////////////
//
// License text...
//
////////////////////////////////////////////////////////////
#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class DiodeBridge
{
public:
    DiodeBridge();

    double IDiodePeak(BCap &bcvalue);
    double IDiodeRMS(InputValue &ivalue, DBridge &dbvalue);
    double IDiodeAVG(DBridge &dbvalue);

    double IDiodeRMSTot(DBridge &dbvalue, InputValue &ivalue);

    double ILoadAVG(DBridge &dbvalue, InputValue &ivalue);
    
    double DiodeCurrentSlope(DBridge &dbvalue, BCap &bcvalue);
    double DiodeConductTime(DBridge &dbvalue);

    double MinPeakInVoltage(InputValue &ivalue);
    double MaxPeakInVoltage(InputValue &ivalue);
};

#endif // DIODEBRIDGE_H
