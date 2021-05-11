/**
  * License text...
*/
#ifndef SWMOSFET_H
#define SWMOSFET_H
#include <QtMath>

struct MosfetProp
{
    int16_t m_vds;//Drain-Source voltage
    int16_t m_idr;//Continuous drain current
    double m_qg;//Total gate charge
    double m_coss;//Output capacitance
    float m_rdson;//Drain-Source on-state resistance
};

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
             int16_t fsw, float mdc, double leakind):
        in_max_pk_voltage(vmaxrmsin),in_min_pk_voltage(vminrmsin),
        actual_volt_reflected(vref), voltage_spike(spv),
        efficiency(eff), power_out_max(pout),
        freq_switch(fsw), actual_max_duty_cycle(mdc), leakage_induct(leakind)
    {}
    inline double swMosfetVoltageMax() const;
    inline double swMosfetVoltageNom() const;
    inline double swMosfetCurrent(double leakage_induct, double primary_induct) const;
    inline double swMosfetTotPeriod() const {return 1./freq_switch;}
    inline double swMosfetOnTime(double primary_induct, double in_volt_rms) const;
    inline double swMosfetOffTime(double sw_on_time);
    inline double swMosfetRiseTime(const MosfetProp &mp) const;
    inline double swMosfetConductLoss(MosfetProp &mp) const;
    inline double swMosfetDriveLoss(MosfetProp &mp) const;
    inline double swMosfetSwitchLoss(MosfetProp &mp) const;
    inline double swMosfetCapacitLoss(MosfetProp &mp) const;
    inline double swMosfetTotalLoss(MosfetProp &mp) const;
    void setCurrValues(float rmscp, float pkcp);

    inline double clVoltageMax() const;
    inline double clPowerDiss() const;
    inline double clResValue() const;
    void setSnubVoltRipple(double vrp);
    inline double clCapValue() const;

    inline double csCurrRes() const;
    void setVoltCurrSens(double csv);
    inline double csCurrResLoss() const;
private:
    int16_t in_max_pk_voltage;
    int16_t in_min_pk_voltage;
    int16_t actual_volt_reflected;
    int16_t voltage_spike;
    float efficiency;
    float power_out_max;
    int16_t freq_switch;
    float actual_max_duty_cycle;
    double leakage_induct;

    float curr_primary_rms;
    float curr_primary_peak;

    double clCurTsPk;
    double clVolRip;
    double csVoltCs;
    inline double swLeakageInduct(double leakage_induct, double primary_induct) const;
    void clCurrPeakTime(double leakage_induct, double primary_induct, double turn_rat, double n_out_volt);
};

#endif // SWMOSFET_H
