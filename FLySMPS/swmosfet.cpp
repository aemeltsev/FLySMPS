/**
  * License text...
*/
#include "swmosfet.h"

SwMosfet::SwMosfet()
{

}
/**
  * @brief estimated voltage of switch not considering spike
  * @param transf, bridge struct ref
  * @retval nom voltage value
  */
double SwMosfet::swMosfetVoltageNom(DBridge &dbvalue, const FBPT &fbptval)
{
    return dbvalue.in_max_rms_voltage + fbptval.actual_volt_reflected;
}
/**
  * @brief estimated voltage stress of switch
  * @param input, fet struct ref
  * @retval max voltage value
  */
double SwMosfet::swMosfetVoltageMax(PMosfet &pmvalue, InputValue &ivalue)
{
    return pmvalue.mosfet_voltage_nom + ivalue.voltage_spike;
}
/**
  * @brief estimated current in switch
  * @param input, transf, bridge struct ref
  * @retval switch current
  */
double SwMosfet::swMosfetCurrent(const FBPT &fbptval, InputValue &ivalue, DBridge &dbvalue)
{
    double time_switch = (1/ivalue.freq_switch);
    return (ivalue.power_out_max/(ivalue.eff * dbvalue.in_min_rms_voltage * fbptval.actual_max_duty_cycle)) + ((dbvalue.in_min_rms_voltage * fbptval.actual_max_duty_cycle * time_switch)/swLeakageInduct(fbptval, ivalue));
}
/**
  * @brief estimated fet vds rise and fall time
  * @param none
  * @retval rise time
  */
double SwMosfet::swMosfetRiseTime()
{
    return Qg * (2./Idrv);
}
/**
  * @brief estimated of conduction losses
  * @param transf struct ptr
  * @retval conduction losses
  */
double SwMosfet::swMosfetConductLoss(const FBPT &fbptval)
{
    return pow(fbptval.curr_primary_rms, 2) * RDSon;
}
/**
  * @brief estimated fet power loss by driving the fet’s gate
  * @param input struct ptr
  * @retval loss by driving the gate
  */
double SwMosfet::swMosfetDriveLoss(InputValue &ivalue)
{
    return Vdrv * Qg * ivalue.freq_switch;
}
/**
  * @brief estimated fet average switching loss
  * @param fet, input, transf struct ptr
  * @retval switching loss
  */
double SwMosfet::swMosfetSwitchLoss(InputValue &ivalue, const FBPT &fbptval, PMosfet &pmvalue)
{
    return ((Coss*pow(pmvalue.mosfet_voltage_max, 2)*ivalue.freq_switch)/2.) + (pmvalue.mosfet_voltage_max*fbptval.curr_primary_peak*pmvalue.mosfet_rise_time*ivalue.freq_switch);
}
/**
  * @brief estimated fet coss power dissipation
  * @param fet and input struct ptr
  * @retval coss power dissipation
  */
double SwMosfet::swMosfetCapacitLoss(PMosfet &pmvalue, InputValue &ivalue)
{
    return (Coss*pow(pmvalue.mosfet_voltage_max, 2)*ivalue.freq_switch)/2.;
}
/**
  * @brief multiply of all losses values
  * @param fet struct ptr
  * @retval total losses
  */
double SwMosfet::swMosfetTotalLoss(PMosfet &pmvalue)
{
    return (pmvalue.mosfet_conduct_loss)*(pmvalue.mosfet_drive_loss)*(pmvalue.mosfet_switch_loss)*(pmvalue.mosfet_capacit_loss);
}
/**
  * @brief set property values for mosfet switch
  * @param rdson - drain-source on-state resistance
  * @param qg - total gate charge
  * @param coss - output capacitance
  * @param idrive - continuous gate current
  * @param vdrive - gate-drive voltage
  * @retval none
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
  * @brief calculate custom gate current
  * @param freq_switch - operating frequency
  * @retval continuous gate current
  */
double SwMosfet::swMosfetCustomIdrv(InputValue &ivalue)
{
    return Qg*ivalue.freq_switch;
}
/**
  * @brief get current value
  * @param input struct ptr
  * @retval continuous gate current
  */
double SwMosfet::getCustomIdrv(InputValue &ivalue)
{
    return swMosfetCustomIdrv(ivalue);
}
/**
  * @brief calculate leakage inductance percentage
  * @param leakage perc && calculated induct primary size
  * @retval leakage inductance
  */
double SwMosfet::swLeakageInduct(const FBPT &fbptval, InputValue &ivalue)
{
    return ivalue.leakage_induct * fbptval.primary_induct;
}
/**
  * @brief calculate snubber capacitor voltage
  * @param mosfet max voltage && rms voltage && reflected voltage
  * @retval snubber voltage
  */
double SwMosfet::clVoltageMax(PMosfet &pmvalue, InputValue &ivalue, DBridge &dbvalue, const FBPT &fbptval)
{
    return swMosfetVoltageMax(pmvalue, ivalue) - dbvalue.in_max_rms_voltage - fbptval.actual_volt_reflected;
}
/**
  * @brief the on-time (tSn) of the snubber diode
  * @param leak ind && primary peak current
  * @param TurnRat - ref. on the turns ratio value
  * @param NOutVolt - ref. on the n-th output side with the max voltage values
  * @retval none
  */
void SwMosfet::clCurrPeakTime(const FBPT &fbptval, InputValue &ivalue, double &TurnRat, double &NOutVolt)
{
    clCurTsPk = (swLeakageInduct(fbptval, ivalue)/(TurnRat*NOutVolt))*fbptval.curr_primary_peak;
}
/**
  * @brief the power dissipated in the snubber circuit
  * @param volt snub && peak curr && sw freq && on-time diode
  * @retval power diss val
  */
double SwMosfet::clPowerDiss(PMosfet &pmvalue, const FBPT &fbptval, InputValue &ivalue)
{
    return ((pmvalue.snubber_voltage_max * fbptval.curr_primary_peak * clCurTsPk * ivalue.freq_switch)/2.);
}
/**
  * @brief the snubber resistor value
  * @param snubb^2/pwr_diss
  * @retval resistor value
  */
double SwMosfet::clResValue(PMosfet &pmvalue)
{
    return pow(pmvalue.snubber_voltage_max, 2)/pmvalue.snubber_pwr_diss;
}
/**
  * @brief the set maximum ripple of the snubber cap voltage
  * @param vrp - 5~10% ripple is reasonable
  * @retval none
  */
void SwMosfet::setSnubVoltRipple(double vrp)
{
    clVolRip = vrp;
}
/**
  * @brief the snubber capacitance value
  * @param voltage max && ripple vol && snubb res && freq sw
  * @retval capacitance value
  */
double SwMosfet::clCapValue(PMosfet &pmvalue, InputValue &ivalue)
{
    return pmvalue.snubber_voltage_max/(clVolRip * pmvalue.snubber_res_value * ivalue.freq_switch);
}
/**
  * @brief set the maximum amplitude of the isense pin
  * @param csv - the typical valuesthe typical values current-sense voltage
  * @retval none
  */
void SwMosfet::setVoltCurrSens(double csv)
{
    csVoltCs = csv;
}
/**
  * @brief the value of current resistor
  * @param current peak prim side && cs voltage
  * @retval the res value
  */
double SwMosfet::csCurrRes(const FBPT &fbptval)
{
    return csVoltCs/fbptval.curr_primary_peak;
}
/**
  * @brief the current sense resistor loss
  * @param rms primary curr && resistor value
  * @retval the sense resistor loss
  */
double SwMosfet::csCurrResLoss(const FBPT &fbptval, PMosfet &pmvalue)
{
    return pow(fbptval.curr_primary_rms, 2) * pmvalue.curr_sense_res;
}
