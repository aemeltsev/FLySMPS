/**
  * License text...
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
double SwMosfet::swMosfetCurrent(double leakage_induct, double primary_induct) const
{
    double time_switch = (1/freq_switch);
    return (static_cast<double>(power_out_max)/static_cast<double>(efficiency * in_min_pk_voltage * actual_max_duty_cycle)) + ((static_cast<double>(in_min_pk_voltage * actual_max_duty_cycle) * time_switch)/swLeakageInduct(leakage_induct, primary_induct));
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
double SwMosfet::swMosfetConductLoss(MosfetProp &mp) const
{
    return std::pow(curr_primary_rms, 2) * mp.m_rdson;
}

/**
 * @brief swMosfetDriveLoss - estimated fet power loss by driving the fet’s gate
 * @return loss by driving the gate
 */
double SwMosfet::swMosfetDriveLoss(MosfetProp &mp) const
{
    return mp.m_vds * mp.m_qg * freq_switch;
}

/**
 * @brief swMosfetSwitchLoss - estimated fet average switching loss
 * @return switching loss
 */
double SwMosfet::swMosfetSwitchLoss(MosfetProp &mp) const
{
    return ((mp.m_coss*qPow(swMosfetVoltageMax(), 2)*freq_switch)/2.) + (swMosfetVoltageMax()*static_cast<double>(curr_primary_peak)*swMosfetRiseTime(mp)*freq_switch);
}

/**
 * @brief swMosfetCapacitLoss - estimated fet coss power dissipation
 * @return coss power dissipation
 */
double SwMosfet::swMosfetCapacitLoss(MosfetProp &mp) const
{
    return (mp.m_coss*std::pow(swMosfetVoltageMax(), 2)*freq_switch)/2.;
}

/**
 * @brief swMosfetTotalLoss - multiply of all losses values
 * @return total losses
 */
double SwMosfet::swMosfetTotalLoss(MosfetProp &mp) const
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
 * @brief swLeakageInduct - calculate leakage inductance percentage
 * @param leakage_induct - leakage perc
 * @param primary_induct - calculated induct primary size
 * @return leakage inductance
 */
double SwMosfet::swLeakageInduct(double leakage_induct, double primary_induct) const
{
    return leakage_induct * primary_induct;
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
 * @brief clCurrPeakTime - the on-time (tSn) of the snubber diode
 * @param leakage_induct - leakage perc
 * @param primary_induct - calculated induct primary size
 * @param turn_rat - on the turns ratio value
 * @param n_out_volt - the n-th output side with the max voltage values
 */
void SwMosfet::clCurrPeakTime(double leakage_induct, double primary_induct, double turn_rat, double n_out_volt)
{
    clCurTsPk = (swLeakageInduct(leakage_induct, primary_induct)/(turn_rat*n_out_volt))*static_cast<double>(curr_primary_peak);
}

/**
 * @brief SwMosfet::clPowerDiss - the power dissipated in the snubber circuit
 * @return power diss val
 */
double SwMosfet::clPowerDiss() const
{
    return ((clVoltageMax() * static_cast<double>(curr_primary_peak) * clCurTsPk * freq_switch)/2.);
}

/**
 * @brief clResValue - the snubber resistor value
 * @return resistor value
 */
double SwMosfet::clResValue() const
{
    return std::pow(clVoltageMax(), 2)/clPowerDiss();
}

/**
  * @brief setSnubVoltRipple - the set maximum ripple of the snubber cap voltage
  * @param vrp - 5~10% ripple is reasonable
  */
void SwMosfet::setSnubVoltRipple(double vrp)
{
    clVolRip = vrp;
}

/**
 * @brief clCapValue - the snubber capacitance value
 * @return capacitance value
 */
double SwMosfet::clCapValue() const
{
    return clVoltageMax()/(clVolRip * clResValue() * freq_switch);
}

/**
  * @brief setVoltCurrSens - set the maximum amplitude of the isense pin
  * @param csv - the typical values current-sense voltage
  */
void SwMosfet::setVoltCurrSens(double csv)
{
    csVoltCs = csv;
}

/**
 * @brief csCurrRes - the value of current resistor
 * @return the res value
 */
double SwMosfet::csCurrRes() const
{
    return csVoltCs/static_cast<double>(curr_primary_peak);
}

/**
 * @brief SwMosfet::csCurrResLoss - the current sense resistor loss
 * @return the sense resistor loss
 */
double SwMosfet::csCurrResLoss() const
{
    return std::pow(curr_primary_rms, 2) * csCurrRes();
}
