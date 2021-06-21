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
             int16_t fsw, float mdc, double leakind)
        :in_max_pk_voltage(vmaxrmsin)
        ,in_min_pk_voltage(vminrmsin)
        ,actual_volt_reflected(vref)
        ,voltage_spike(spv)
        ,freq_switch(fsw)
        ,efficiency(eff)
        ,power_out_max(pout)
        ,actual_max_duty_cycle(mdc)
        ,leakage_induct(leakind)
    {}

    /**
     * @brief swMosfetVoltageMax - estimated voltage stress of switch
     * @return max voltage value
     */
    double swMosfetVoltageMax() const
    {
        return swMosfetVoltageNom() + voltage_spike;
    }

    /**
     * @brief swMosfetVoltageNom - estimated voltage of switch not considering spike
     * @return nom voltage value
     */
    double swMosfetVoltageNom() const
    {
        return in_max_pk_voltage + actual_volt_reflected;
    }

    /**
     * @brief swMosfetCurrent - estimated current in switch
     * @param leakage_induct - leakage perc
     * @param primary_induct - calculated induct primary size
     * @return switch current
     */
    double swMosfetCurrent() const
    {
        double time_switch = (1/freq_switch);
        return (static_cast<double>(power_out_max)/static_cast<double>(efficiency * in_min_pk_voltage * actual_max_duty_cycle)) + ((static_cast<double>(in_min_pk_voltage * actual_max_duty_cycle) * time_switch)/leakage_induct);
    }

    inline double swMosfetTotPeriod() const
    {
        return 1./freq_switch;
    }

    inline double swMosfetOnTime(double primary_induct, double in_volt_rms) const
    {
        return (primary_induct * static_cast<double>(power_out_max))/(static_cast<double>(efficiency * actual_max_duty_cycle) * qPow(in_volt_rms, 2));
    }

    inline double swMosfetOffTime(double sw_on_time)
    {
        return (static_cast<double>(actual_max_duty_cycle) * sw_on_time) - sw_on_time;
    }

    /**
     * @brief swMosfetRiseTime - estimated fet vds rise and fall time
     * @return rise time
     */
    double swMosfetRiseTime(const MosfetProp &mp) const
    {
        return mp.m_qg * (2./mp.m_idr);
    }

    /**
     * @brief swMosfetConductLoss - estimated of conduction losses
     * @retval conduction losses
     */
    double swMosfetConductLoss(const MosfetProp &mp) const
    {
        return qPow(static_cast<double>(curr_primary_rms), 2) * static_cast<double>(mp.m_rdson);
    }

    /**
     * @brief swMosfetDriveLoss - estimated fet power loss by driving the fet’s gate
     * @return loss by driving the gate
     */
    double swMosfetDriveLoss(const MosfetProp &mp) const
    {
        return mp.m_vgs * mp.m_qg * freq_switch;
    }

    /**
     * @brief swMosfetSwitchLoss - estimated fet average switching loss
     * @return switching loss
     */
    double swMosfetSwitchLoss(const MosfetProp &mp) const
    {
        return ((mp.m_coss*qPow(swMosfetVoltageMax(), 2)*freq_switch)/2.) + (swMosfetVoltageMax()*static_cast<double>(curr_primary_peak)*swMosfetRiseTime(mp)*freq_switch);
    }

    /**
     * @brief swMosfetCapacitLoss - estimated fet coss power dissipation
     * @return coss power dissipation
     */
    double swMosfetCapacitLoss(const MosfetProp &mp) const
    {
        return (mp.m_coss*std::pow(swMosfetVoltageMax(), 2)*freq_switch)/2.;
    }

    /**
     * @brief swMosfetTotalLoss - multiply of all losses values
     * @return total losses
     */
    double swMosfetTotalLoss(const MosfetProp &mp) const
    {
        return swMosfetConductLoss(mp)*swMosfetDriveLoss(mp)*swMosfetSwitchLoss(mp)*swMosfetCapacitLoss(mp);
    }

    /**
     * @brief setCurrValues -
     * @param rmscp
     * @param pkcp
     */
    void setCurrValues(float rmscp, float pkcp)
    {
        curr_primary_rms = rmscp;
        curr_primary_peak = pkcp;
    }

    /**
     * @brief clVoltageMax - calculate snubber capacitor voltage
     * @return snubber voltage
     */
    double clVoltageMax() const
    {
        return swMosfetVoltageMax() - in_max_pk_voltage - actual_volt_reflected;
    }

    /**
     * @brief SwMosfet::clPowerDiss - the power dissipated in the snubber circuit
     * @return power diss val
     */
    double clPowerDiss(const ClampCSProp &ccsp) const
    {
        /**< the on-time (tSn) of the snubber diode */
        auto clCurTsPk = (leakage_induct / (static_cast<double>(ccsp.cl_turn_rat * ccsp.cl_first_out_volt))) * static_cast<double>(curr_primary_peak);
        return ((clVoltageMax() * static_cast<double>(curr_primary_peak) * clCurTsPk * freq_switch)/2.);
    }

    /**
     * @brief clResValue - the snubber resistor value
     * @return resistor value
     */
    double clResValue(const ClampCSProp &ccsp) const
    {
        return std::pow(clVoltageMax(), 2)/clPowerDiss(ccsp);
    }

    /**
     * @brief clCapValue - the snubber capacitance value
     * @return capacitance value
     */
    double clCapValue(const ClampCSProp &ccsp) const
    {
        return clVoltageMax()/(ccsp.cl_vol_rip * clResValue(ccsp) * freq_switch);
    }

    /**
     * @brief csCurrRes - the value of current resistor
     * @return the res value
     */
    double csCurrRes(const ClampCSProp &ccsp) const
    {
        return ccsp.cs_volt/static_cast<double>(curr_primary_peak);
    }

    /**
     * @brief SwMosfet::csCurrResLoss - the current sense resistor loss
     * @return the sense resistor loss
     */
    double csCurrResLoss(const ClampCSProp &ccsp) const
    {
        return std::pow(curr_primary_rms, 2) * csCurrRes(ccsp);
    }
};

#endif // SWMOSFET_H
