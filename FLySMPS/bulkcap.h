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

    double CapValue(InputValue *ivalue);

    double ILoadMax(InputValue* ivalue, DBridge* dbvalue);
    double ILoadMin(InputValue* ivalue, DBridge* dbvalue);
    double IBulkCapPeak(BCap *bcvalue, InputValue *ivalue);
    double IBulkCapRMS(DBridge *dbvalue, InputValue* ivalue);

    double VMinInp(BCap* bcvalue, InputValue* ivalue);
    double VDCMin(BCap *bcvalue, InputValue *ivalue);

private:
    double VDOut(InputValue* ivalue);
    double VRRMS(InputValue* ivalue);

};

#endif // BULKCAP_H
