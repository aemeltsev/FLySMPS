#ifndef SWMOSFET_H
#define SWMOSFET_H
#include <cmath>
#include <structdata.h>

class SwMosfet
{
public:
    SwMosfet();
    double swMosfetVoltage();
    double swMosfetCurrent();
    double swMosfetConductLoss();
    double swMosfetDriveLoss();
    double swMosfetSwitchLoss();
    double swMosfetCapacitLoss();
    double swMosfetTotalLoss();
private:

};

#endif // SWMOSFET_H
