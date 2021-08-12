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

/**
 * @brief The MosfetProp struct
 *        For V_miller see:
 *        AN608A.-Power MOSFET Basics: Understanding Gate Chargeand Using it to Assess Switching Performance
 *        https://electronics.stackexchange.com/questions/437680/how-to-calculate-gate-charge-time-for-a-mosfet
 *        https://electronics.stackexchange.com/questions/66660/why-is-the-gate-charge-curve-miller-plateau-of-mosfets-dependent-on-vds
 *        For complete power stage design include fet properties estimate see:
 *        SLVUBB4A.-Power Stage Designer.-TI.2018
 */
struct MosfetProp
{
    int16_t m_vgs;//Gate-Source voltage
    float m_idr;//Continuous drain current

    double m_qg;//Total gate charge
    double m_qgd;//Gate-Drain charge
    double m_qgs;//Gate-Source charge
    double m_rgate;//Gate resistance

    double m_vmill;//Miller voltage

    float m_fet_cur_max;//Current fet ds max
    float m_fet_cur_min;//Current fet ds min

    double m_coss;//Output capacitance
    float m_rdson;//Drain-Source on-state resistance
};

struct ClampCSProp
{
    int16_t cl_first_out_volt;//First controlled secondary side voltage value include diode drop
    float cl_turn_rat;//Common turn ratio value
    double leakage_induct;//
    double cl_vol_rip;//5~10% ripple is reasonable
    double cs_volt;//The typical values current-sense voltage

};

class SwMosfet
{
private:
    uint16_t in_max_pk_voltage;
    uint16_t voltage_spike;
    uint32_t freq_switch;
    double prim_induct;
    double prim_cur_pkp;
    double actual_volt_reflected;

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
     * @param fsw - power switching frequency
     */
    SwMosfet(uint16_t vmaxrmsin,
             uint16_t spv,
             uint32_t fsw,
             double vref,
             double pind,
             double cpkp)
        :in_max_pk_voltage(vmaxrmsin)
        ,voltage_spike(spv)
        ,freq_switch(fsw)
        ,prim_induct(pind)
        ,prim_cur_pkp(cpkp)
        ,actual_volt_reflected(vref)
    {}

    /**
     * @brief swMosfetVoltageNom - estimated voltage of switch not considering spike
     * @return nom voltage value
     */
    inline double swMosfetVoltageNom() const
    {
        return in_max_pk_voltage + actual_volt_reflected;
    }

    /**
     * @brief swMosfetVoltageMax - estimated voltage stress of switch
     * @return max voltage value
     */
    inline double swMosfetVoltageMax() const
    {
        return swMosfetVoltageNom() + voltage_spike;
    }

    /**
     * @brief swMosfetRiseTime
     * @param mprp
     * @return
     */
    inline double swMosfetRiseTime(const MosfetProp& mprp) const
    {
       double gd_coeff = (mprp.m_qgd * mprp.m_rgate) / (mprp.m_vgs - mprp.m_vmill);
       double gs_coeff = ((mprp.m_qgs - mprp.m_qg) * mprp.m_rgate) / (mprp.m_vgs - (mprp.m_vmill / 2.) - (mprp.m_vgs / 2.));
       return qAbs(gs_coeff + gd_coeff);
    }

    /**
     * @brief swMosfetFallTime
     * @param mprp
     * @return
     */
    inline double swMosfetFallTime(const MosfetProp& mprp) const
    {
        double gd_coeff = (mprp.m_qgd * mprp.m_rgate) / mprp.m_vmill;
        double gs_coeff = ((mprp.m_qgs - mprp.m_qg) * mprp.m_rgate) / ((mprp.m_vmill / 2.) + (mprp.m_vgs / 2.));
        return gd_coeff + gs_coeff;
    }

    /**
     * @brief swMosfetOnTime
     * @return
     */
    inline double swMosfetOnTime(const MosfetProp& mprp) const
    {
        return ((prim_induct * prim_cur_pkp) / in_max_pk_voltage) + swMosfetRiseTime(mprp);
    }

    /**
     * @brief swMosfetOffTime
     * @param mp
     * @return
     */
    inline double swMosfetOffTime(const MosfetProp &mprp, float trat, float vout) const
    {
        return ((prim_induct * prim_cur_pkp) / static_cast<double>((trat * vout))) + swMosfetFallTime(mprp);
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
        double t_coeff = (swMosfetRiseTime(mp) * static_cast<double>(mp.m_fet_cur_min)) + (swMosfetFallTime(mp) * static_cast<double>(mp.m_fet_cur_max));
        return swMosfetVoltageNom() * (freq_switch/2.) * t_coeff;
    }

    /**
     * @brief swMosfetCapacitLoss - estimated fet coss power dissipation
     * @return coss power dissipation
     */
    double swMosfetCapacitLoss(const MosfetProp &mp) const
    {
        return (mp.m_coss * qPow(swMosfetVoltageMax(), 2)*freq_switch)/2.;
    }

    /**
     * @brief swMosfetTotalLoss - multiply of all losses values
     * @return total losses
     */
    double swMosfetTotalLoss(const MosfetProp &mp) const
    {
        return swMosfetConductLoss(mp) + swMosfetDriveLoss(mp) + swMosfetSwitchLoss(mp) + swMosfetCapacitLoss(mp);
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
     * @brief clCapValue - the snubber capacitance value
     * @return capacitance value
     */
    double clCapValue(const ClampCSProp &ccsp) const
    {
        double num = ccsp.leakage_induct * qPow(static_cast<double>(curr_primary_peak), 2);
        double dnm = qPow((actual_volt_reflected + voltage_spike), 2) - qPow(actual_volt_reflected, 2);
        return num / dnm;
        //return clVoltageMax()/(ccsp.cl_vol_rip * clResValue(ccsp) * freq_switch);
    }

    /**
     * @brief clResValue - the snubber resistor value
     * @return resistor value
     */
    double clResValue(const ClampCSProp &ccsp) const
    {
        double dnm = freq_switch * clCapValue(ccsp) * std::log2(1 + (voltage_spike / actual_volt_reflected));
        return 1. / dnm;
        //return qPow(clVoltageMax(), 2)/clPowerDiss(ccsp);
    }

    /**
     * @brief SwMosfet::clPowerDiss - the power dissipated in the snubber circuit
     * @return power diss val
     */
    double clPowerDiss(const ClampCSProp &ccsp) const
    {
        double coeff = 0.5 * ccsp.leakage_induct * qPow(static_cast<double>(curr_primary_peak), 2) * freq_switch;
        return (qPow(actual_volt_reflected, 2) / clResValue(ccsp)) + coeff;
        /**< the on-time (tSn) of the snubber diode */
        //auto clCurTsPk = (leakage_induct / (static_cast<double>(ccsp.cl_turn_rat * ccsp.cl_first_out_volt))) * static_cast<double>(curr_primary_peak);
        //return ((clVoltageMax() * static_cast<double>(curr_primary_peak) * clCurTsPk * freq_switch)/2.);
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
        return qPow(curr_primary_rms, 2) * csCurrRes(ccsp);
    }
};

#endif // SWMOSFET_H
