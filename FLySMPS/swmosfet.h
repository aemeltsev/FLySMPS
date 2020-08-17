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
    SwMosfet(int16_t vmaxrmsin, int16_t vminrmsin,
             int16_t vref, int16_t spv,
             float eff, float pout,
             int16_t fsw, float mdc):
        in_max_rms_voltage(vmaxrmsin),in_min_rms_voltage(vminrmsin),
        actual_volt_reflected(vref), voltage_spike(spv),
        efficiency(eff), power_out_max(pout),
        freq_switch(fsw), actual_max_duty_cycle(mdc)
    {}
    inline double swMosfetVoltageMax() const;
    inline double swMosfetVoltageNom() const;
    inline double swMosfetCurrent(const FBPT &fbptval, InputValue &ivalue);
    inline double swMosfetRiseTime() const;
    inline double swMosfetConductLoss(const FBPT &fbptval);
    inline double swMosfetDriveLoss(InputValue &ivalue);
    inline double swMosfetSwitchLoss(InputValue &ivalue, const FBPT &fbptval, PMosfet &pmvalue);
    inline double swMosfetCapacitLoss(PMosfet &pmvalue, InputValue &ivalue);
    inline double swMosfetTotalLoss(PMosfet &pmvalue);
    void setSWValue(double rdson, double qg,
                    double coss, double idrive,
                    int16_t vdrive);
    inline double getCustomIdrv() const;

    inline double clVoltageMax(PMosfet &pmvalue, InputValue &ivalue, DBridge &dbvalue, const FBPT &fbptval);
    inline double clPowerDiss(PMosfet &pmvalue, const FBPT &fbptval,InputValue &ivalue);
    inline double clResValue(PMosfet &pmvalue);
    void setSnubVoltRipple(double vrp);
    inline double clCapValue(PMosfet &pmvalue, InputValue &ivalue);

    inline double csCurrRes(const FBPT &fbptval);
    void setVoltCurrSens(double csv);
    inline double csCurrResLoss(const FBPT &fbptval, PMosfet &pmvalue);
private:
    int16_t in_max_rms_voltage;
    int16_t in_min_rms_voltage;
    int16_t actual_volt_reflected;
    int16_t voltage_spike;
    float efficiency;
    float power_out_max;
    int16_t freq_switch;
    float actual_max_duty_cycle;

    double RDSon;
    double Qg;
    double Coss;
    double Idrv;
    int16_t Vdrv;
    double clCurTsPk;
    double clVolRip;
    double csVoltCs;
    inline double swMosfetCustomIdrv() const;
    inline double swLeakageInduct(const FBPT &fbptval, InputValue &ivalue);
    void clCurrPeakTime(const FBPT &fbptval, InputValue &ivalue, double &TurnRat, double &NOutVolt);
};

#endif // SWMOSFET_H
