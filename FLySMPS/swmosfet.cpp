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
    return in_max_rms_voltage + actual_volt_reflected;
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
    return (static_cast<double>(power_out_max)/static_cast<double>(efficiency * in_min_rms_voltage * actual_max_duty_cycle)) + ((static_cast<double>(in_min_rms_voltage * actual_max_duty_cycle) * time_switch)/swLeakageInduct(leakage_induct, primary_induct));
}

/**
 * @brief swMosfetRiseTime - estimated fet vds rise and fall time
 * @return rise time
 */
double SwMosfet::swMosfetRiseTime() const
{
    return Qg * (2./Idrv);
}

/**
 * @brief swMosfetConductLoss - estimated of conduction losses
 * @retval conduction losses
 */
double SwMosfet::swMosfetConductLoss() const
{
    return std::pow(curr_primary_rms, 2) * RDSon;
}

/**
 * @brief swMosfetDriveLoss - estimated fet power loss by driving the fet’s gate
 * @return loss by driving the gate
 */
double SwMosfet::swMosfetDriveLoss() const
{
    return Vdrv * Qg * freq_switch;
}

/**
 * @brief swMosfetSwitchLoss - estimated fet average switching loss
 * @return switching loss
 */
double SwMosfet::swMosfetSwitchLoss() const
{
    return ((Coss*std::pow(swMosfetVoltageMax(), 2)*freq_switch)/2.) + (swMosfetVoltageMax()*static_cast<double>(curr_primary_peak)*swMosfetRiseTime()*freq_switch);
}

/**
 * @brief swMosfetCapacitLoss - estimated fet coss power dissipation
 * @return coss power dissipation
 */
double SwMosfet::swMosfetCapacitLoss() const
{
    return (Coss*std::pow(swMosfetVoltageMax(), 2)*freq_switch)/2.;
}

/**
 * @brief swMosfetTotalLoss - multiply of all losses values
 * @return total losses
 */
double SwMosfet::swMosfetTotalLoss() const
{
    return swMosfetConductLoss()*swMosfetDriveLoss()*swMosfetSwitchLoss()*swMosfetCapacitLoss();
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
 * @brief setSWValue - set property values for mosfet switch
 * @param rdson - drain-source on-state resistance
 * @param qg - total gate charge
 * @param coss - output capacitance
 * @param idrive - continuous gate current
 * @param vdrive - gate-drive voltage
 */
void SwMosfet::setSWValue(double rdson, double qg, double coss, double idrive, int16_t vdrive)
{
    RDSon = rdson;
    Qg = qg;
    Coss = coss;
    Idrv = idrive;
    Vdrv = vdrive;
}

/**
 * @brief swMosfetCustomIdrv - calculate custom gate current
 * @return continuous gate current
 */
double SwMosfet::swMosfetCustomIdrv() const
{
    return Qg*freq_switch;
}

/**
 * @brief getCustomIdrv - get current value
 * @return continuous gate current
 */
double SwMosfet::getCustomIdrv() const
{
    return swMosfetCustomIdrv();
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
    return swMosfetVoltageMax() - in_max_rms_voltage - actual_volt_reflected;
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
