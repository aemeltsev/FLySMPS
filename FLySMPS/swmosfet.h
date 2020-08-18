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
    /**
     * @brief SwMosfet - class for power switch
     * @param vmaxrmsin - maximum rms input voltage
     * @param vminrmsin - minimum rms input voltage
     * @param vref - actual reflected voltage pr. side
     * @param spv - max voltage spike value, for pow switch
     * @param eff - efficiency
     * @param pout - all power output
     * @param fsw - power switching frequency
     * @param mdc - max duty cucle
     */
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
    inline double swMosfetCurrent(double leakage_induct, double primary_induct) const;
    inline double swMosfetRiseTime() const;
    inline double swMosfetConductLoss() const;
    inline double swMosfetDriveLoss() const;
    inline double swMosfetSwitchLoss() const;
    inline double swMosfetCapacitLoss() const;
    inline double swMosfetTotalLoss() const;
    void setCurrValues(float rmscp, float pkcp);
    void setSWValue(double rdson, double qg,
                    double coss, double idrive,
                    int16_t vdrive);
    inline double getCustomIdrv() const;

    inline double clVoltageMax() const;
    inline double clPowerDiss() const;
    inline double clResValue() const;
    void setSnubVoltRipple(double vrp);
    inline double clCapValue() const;

    inline double csCurrRes() const;
    void setVoltCurrSens(double csv);
    inline double csCurrResLoss() const;
private:
    int16_t in_max_rms_voltage;
    int16_t in_min_rms_voltage;
    int16_t actual_volt_reflected;
    int16_t voltage_spike;
    float efficiency;
    float power_out_max;
    int16_t freq_switch;
    float actual_max_duty_cycle;

    float curr_primary_rms;
    float curr_primary_peak;

    double RDSon;
    double Qg;
    double Coss;
    double Idrv;
    int16_t Vdrv;
    double clCurTsPk;
    double clVolRip;
    double csVoltCs;
    inline double swMosfetCustomIdrv() const;
    inline double swLeakageInduct(double leakage_induct, double primary_induct) const;
    void clCurrPeakTime(double leakage_induct, double primary_induct, double turn_rat, double n_out_volt);
};

#endif // SWMOSFET_H
