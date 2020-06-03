/**
  * License text...
*/
#ifndef SWMOSFET_H
#define SWMOSFET_H
#include <cmath>
#include <structdata.h>

class SwMosfet
{
public:
    SwMosfet();
    double swMosfetVoltageMax(PMosfet &pmvalue, InputValue &ivalue);
    double swMosfetVoltageNom(DBridge &dbvalue, FBTransformer &fbtvalue);
    double swMosfetCurrent(FBTransformer &fbtvalue, InputValue &ivalue, DBridge &dbvalue);
    double swMosfetRiseTime();
    double swMosfetConductLoss(FBTransformer &fbtvalue);
    double swMosfetDriveLoss(InputValue &ivalue);
    double swMosfetSwitchLoss(InputValue &ivalue, FBTransformer &fbtvalue, PMosfet &pmvalue);
    double swMosfetCapacitLoss(PMosfet &pmvalue, InputValue &ivalue);
    double swMosfetTotalLoss(PMosfet &pmvalue);
    void setSWValue(double rdson, double qg, double coss, double idrive, int16_t vdrive);
    double getCustomIdrv(InputValue &ivalue);

private:
    double RDSon;
    double Qg;
    double Coss;
    double Idrv;
    int16_t Vdrv;
    double swMosfetCustomIdrv(InputValue &ivalue);
    double swLeakageInduct(FBTransformer &fbtvalue, InputValue &ivalue);

};

#endif // SWMOSFET_H
