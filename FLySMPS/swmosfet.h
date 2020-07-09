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
    double swMosfetVoltageNom(DBridge &dbvalue, const FBPT &fbptval);
    double swMosfetCurrent(const FBPT &fbptval, InputValue &ivalue, DBridge &dbvalue);
    double swMosfetRiseTime();
    double swMosfetConductLoss(const FBPT &fbptval);
    double swMosfetDriveLoss(InputValue &ivalue);
    double swMosfetSwitchLoss(InputValue &ivalue, const FBPT &fbptval, PMosfet &pmvalue);
    double swMosfetCapacitLoss(PMosfet &pmvalue, InputValue &ivalue);
    double swMosfetTotalLoss(PMosfet &pmvalue);
    void setSWValue(double rdson, double qg, double coss, double idrive, int16_t vdrive);
    double getCustomIdrv(InputValue &ivalue);

    double clVoltageMax(PMosfet &pmvalue, InputValue &ivalue, DBridge &dbvalue, const FBPT &fbptval);
    double clPowerDiss(PMosfet &pmvalue, const FBPT &fbptval,InputValue &ivalue);
    double clResValue(PMosfet &pmvalue);
    void setSnubVoltRipple(double vrp);
    double clCapValue(PMosfet &pmvalue, InputValue &ivalue);

    double csCurrRes(const FBPT &fbptval);
    void setVoltCurrSens(double csv);
    double csCurrResLoss(const FBPT &fbptval, PMosfet &pmvalue);
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
    double swLeakageInduct(const FBPT &fbptval, InputValue &ivalue);
    void clCurrPeakTime(const FBPT &fbptval, InputValue &ivalue, double &TurnRat, double &NOutVolt);
};

#endif // SWMOSFET_H
