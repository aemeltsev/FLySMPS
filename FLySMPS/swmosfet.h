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

    double clVoltageMax(PMosfet &pmvalue, InputValue &ivalue, DBridge &dbvalue, FBTransformer &fbtvalue);
    double clPowerDiss(PMosfet &pmvalue, FBTransformer &fbtvalue,InputValue &ivalue);
    double clResValue(PMosfet &pmvalue);
    void setSnubVoltRipple(double vrp);
    double clCapValue(PMosfet &pmvalue, InputValue &ivalue);


    double csCurrRes(FBTransformer &fbtvalue);
    void setVoltCurrSens(double csv);
    double csCurrResLoss(FBTransformer &fbtvalue, PMosfet &pmvalue);
private:
    double RDSon;
    double Qg;
    double Coss;
    double Idrv;
    int16_t Vdrv;
    double clCurTsPk;
    double clVolRip;
    double csVoltCs;
    double swMosfetCustomIdrv(InputValue &ivalue);
    double swLeakageInduct(FBTransformer &fbtvalue, InputValue &ivalue);
    void clCurrPeakTime(FBTransformer &fbtvalue, InputValue &ivalue, double &TurnRat, double &NOutVolt);
};

#endif // SWMOSFET_H
