/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef SWMOSFET_H
#define SWMOSFET_H
#include <QtMath>

struct MosfetProp
{
    int16_t m_vgs;//Gate-Source voltage
    int16_t m_idr;//Continuous drain current
    double m_qg;//Total gate charge
    double m_coss;//Output capacitance
    float m_rdson;//Drain-Source on-state resistance
};

struct ClampCSProp
{
    int16_t cl_first_out_volt;//First controlled secondary side voltage value
    float cl_turn_rat;//Common turn ratio value
    double cl_vol_rip;//5~10% ripple is reasonable
    double cs_volt;//The typical values current-sense voltage
};

class SwMosfet
{
public:
    /**
     * @brief SwMosfet - class for power switch.
     *        For losses calculation see more:
     *        Lakkas G.-MOSFET power losses and how they affect power-supply efficiency
     *        Balogh L.-Design And Application Guide For High Speed MOSFET Gate Drive Circuits
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
        freq_switch(fsw), efficiency(eff),
        power_out_max(pout), actual_max_duty_cycle(mdc),
        leakage_induct(leakind)
    {}
    inline double swMosfetVoltageMax() const;
    inline double swMosfetVoltageNom() const;
    inline double swMosfetCurrent() const;
    inline double swMosfetTotPeriod() const {return 1./freq_switch;}
    inline double swMosfetOnTime(double primary_induct, double in_volt_rms) const;
    inline double swMosfetOffTime(double sw_on_time);
    inline double swMosfetRiseTime(const MosfetProp &mp) const;
    inline double swMosfetConductLoss(const MosfetProp &mp) const;
    inline double swMosfetDriveLoss(const MosfetProp &mp) const;
    inline double swMosfetSwitchLoss(const MosfetProp &mp) const;
    inline double swMosfetCapacitLoss(const MosfetProp &mp) const;
    inline double swMosfetTotalLoss(const MosfetProp &mp) const;

    void setCurrValues(float rmscp, float pkcp);
    inline double clVoltageMax() const;
    inline double clPowerDiss(const ClampCSProp &ccsp) const;
    inline double clResValue(const ClampCSProp &ccsp) const;
    inline double clCapValue(const ClampCSProp &ccsp) const;

    inline double csCurrRes(const ClampCSProp &ccsp) const;
    inline double csCurrResLoss(const ClampCSProp &ccsp) const;
private:
    int16_t in_max_pk_voltage;
    int16_t in_min_pk_voltage;
    int16_t actual_volt_reflected;
    int16_t voltage_spike;
    int16_t freq_switch;
    float efficiency;
    float power_out_max;
    float actual_max_duty_cycle;
    double leakage_induct;

    float curr_primary_rms;
    float curr_primary_peak;
};

#endif // SWMOSFET_H
