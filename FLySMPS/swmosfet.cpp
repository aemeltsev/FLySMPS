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

#include "swmosfet.h"

/**
 * @brief swMosfetVoltageNom - estimated voltage of switch not considering spike
 * @return nom voltage value
 */
double SwMosfet::swMosfetVoltageNom() const
{
    return in_max_pk_voltage + actual_volt_reflected;
}

/**
 * @brief swMosfetVoltageMax - estimated voltage stress of switch
 * @return max voltage value
 */
double SwMosfet::swMosfetVoltageMax() const
{
    return swMosfetVoltageNom() + voltage_spike;
}

/**
 * @brief swMosfetCurrent - estimated current in switch
 * @param leakage_induct - leakage perc
 * @param primary_induct - calculated induct primary size
 * @return switch current
 */
double SwMosfet::swMosfetCurrent() const
{
    double time_switch = (1/freq_switch);
    return (static_cast<double>(power_out_max)/static_cast<double>(efficiency * in_min_pk_voltage * actual_max_duty_cycle)) + ((static_cast<double>(in_min_pk_voltage * actual_max_duty_cycle) * time_switch)/leakage_induct);
}

inline double SwMosfet::swMosfetOnTime(double primary_induct, double in_volt_rms) const
{
    return (primary_induct * power_out_max)/(efficiency * actual_max_duty_cycle * qPow(in_volt_rms, 2));
}

inline double SwMosfet::swMosfetOffTime(double sw_on_time)
{
    return (actual_max_duty_cycle * sw_on_time) - sw_on_time;
}

/**
 * @brief swMosfetRiseTime - estimated fet vds rise and fall time
 * @return rise time
 */
double SwMosfet::swMosfetRiseTime(const MosfetProp &mp) const
{
    return mp.m_qg * (2./mp.m_idr);
}

/**
 * @brief swMosfetConductLoss - estimated of conduction losses
 * @retval conduction losses
 */
double SwMosfet::swMosfetConductLoss(const MosfetProp &mp) const
{
    return qPow(curr_primary_rms, 2) * mp.m_rdson;
}

/**
 * @brief swMosfetDriveLoss - estimated fet power loss by driving the fet’s gate
 * @return loss by driving the gate
 */
double SwMosfet::swMosfetDriveLoss(const MosfetProp &mp) const
{
    return mp.m_vgs * mp.m_qg * freq_switch;
}

/**
 * @brief swMosfetSwitchLoss - estimated fet average switching loss
 * @return switching loss
 */
double SwMosfet::swMosfetSwitchLoss(const MosfetProp &mp) const
{
    return ((mp.m_coss*qPow(swMosfetVoltageMax(), 2)*freq_switch)/2.) + (swMosfetVoltageMax()*static_cast<double>(curr_primary_peak)*swMosfetRiseTime(mp)*freq_switch);
}

/**
 * @brief swMosfetCapacitLoss - estimated fet coss power dissipation
 * @return coss power dissipation
 */
double SwMosfet::swMosfetCapacitLoss(const MosfetProp &mp) const
{
    return (mp.m_coss*std::pow(swMosfetVoltageMax(), 2)*freq_switch)/2.;
}

/**
 * @brief swMosfetTotalLoss - multiply of all losses values
 * @return total losses
 */
double SwMosfet::swMosfetTotalLoss(const MosfetProp &mp) const
{
    return swMosfetConductLoss(mp)*swMosfetDriveLoss(mp)*swMosfetSwitchLoss(mp)*swMosfetCapacitLoss(mp);
}

/**
 * @brief setCurrValues -
 * @param rmscp
 * @param pkcp
 */
void SwMosfet::setCurrValues(float rmscp, float pkcp)
{
    curr_primary_rms = rmscp;
    curr_primary_peak = pkcp;
}

/**
 * @brief clVoltageMax - calculate snubber capacitor voltage
 * @return snubber voltage
 */
double SwMosfet::clVoltageMax() const
{
    return swMosfetVoltageMax() - in_max_pk_voltage - actual_volt_reflected;
}

/**
 * @brief SwMosfet::clPowerDiss - the power dissipated in the snubber circuit
 * @return power diss val
 */
double SwMosfet::clPowerDiss(const ClampCSProp &ccsp) const
{
    /**< the on-time (tSn) of the snubber diode */
    auto clCurTsPk = (leakage_induct/(ccsp.cl_turn_rat * ccsp.cl_first_out_volt))*static_cast<double>(curr_primary_peak);
    return ((clVoltageMax() * static_cast<double>(curr_primary_peak) * clCurTsPk * freq_switch)/2.);
}

/**
 * @brief clResValue - the snubber resistor value
 * @return resistor value
 */
double SwMosfet::clResValue(const ClampCSProp &ccsp) const
{
    return std::pow(clVoltageMax(), 2)/clPowerDiss(ccsp);
}

/**
 * @brief clCapValue - the snubber capacitance value
 * @return capacitance value
 */
double SwMosfet::clCapValue(const ClampCSProp &ccsp) const
{
    return clVoltageMax()/(ccsp.cl_vol_rip * clResValue(ccsp) * freq_switch);
}

/**
 * @brief csCurrRes - the value of current resistor
 * @return the res value
 */
double SwMosfet::csCurrRes(const ClampCSProp &ccsp) const
{
    return ccsp.cs_volt/static_cast<double>(curr_primary_peak);
}

/**
 * @brief SwMosfet::csCurrResLoss - the current sense resistor loss
 * @return the sense resistor loss
 */
double SwMosfet::csCurrResLoss(const ClampCSProp &ccsp) const
{
    return std::pow(curr_primary_rms, 2) * csCurrRes(ccsp);
}
