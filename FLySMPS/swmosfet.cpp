#include "swmosfet.h"

SwMosfet::SwMosfet()
{

}
/**
  * @brief estimated voltage of switch not considering spike
  * @param transf, bridge struct ptr
  * @retval nom voltage value
  */
double SwMosfet::swMosfetVoltageNom(DBridge *dbvalue, FBTransformer *fbtvalue)
{
    return dbvalue->in_max_rms_voltage + fbtvalue->actual_volt_reflected;
}
/**
  * @brief estimated voltage stress of switch
  * @param input, fet struct ptr
  * @retval max voltage value
  */
double SwMosfet::swMosfetVoltageMax(PMosfet *pmvalue, InputValue *ivalue)
{
    return pmvalue->mosfet_voltage_nom + ivalue->voltage_spike;
}
/**
  * @brief estimated current in switch
  * @param input, transf, bridge struct ptr
  * @retval switch current
  */
double SwMosfet::swMosfetCurrent(FBTransformer *fbtvalue, InputValue *ivalue, DBridge *dbvalue)
{
    double leakage = ivalue->leakage_induct * fbtvalue->primary_induct;
    double time_switch = (1/ivalue->freq_switch);
    return (ivalue->power_out_max/(ivalue->eff * dbvalue->in_min_rms_voltage * fbtvalue->actual_max_duty_cycle)) + ((dbvalue->in_min_rms_voltage * fbtvalue->actual_max_duty_cycle * time_switch)/leakage);
}
/**
  * @brief estimated fet vds rise and fall time
  * @param none
  * @retval rise time
  */
double SwMosfet::swMosfetRiseTime()
{
    return Qg*(2./Idrv);
}
/**
  * @brief estimated of conduction losses
  * @param transf struct ptr
  * @retval conduction losses
  */
double SwMosfet::swMosfetConductLoss(FBTransformer *fbtvalue)
{
    return pow(fbtvalue->curr_primary_rms, 2) * RDSon;
}
/**
  * @brief estimated fet power loss by driving the fet’s gate
  * @param input struct ptr
  * @retval loss by driving the gate
  */
double SwMosfet::swMosfetDriveLoss(InputValue *ivalue)
{
    return Vdrv*Qg*ivalue->freq_switch;
}
/**
  * @brief estimated fet average switching loss
  * @param fet, input, transf struct ptr
  * @retval switching loss
  */
double SwMosfet::swMosfetSwitchLoss(InputValue *ivalue, FBTransformer *fbtvalue, PMosfet *pmvalue)
{
    return ((Coss*pow(pmvalue->mosfet_voltage_max, 2)*ivalue->freq_switch)/2.) + (pmvalue->mosfet_voltage_max*fbtvalue->curr_primary_peak*pmvalue->mosfet_rise_time*ivalue->freq_switch);
}
/**
  * @brief estimated fet coss power dissipation
  * @param fet and input struct ptr
  * @retval coss power dissipation
  */
double SwMosfet::swMosfetCapacitLoss(PMosfet *pmvalue, InputValue *ivalue)
{
    return (Coss*pow(pmvalue->mosfet_voltage_max, 2)*ivalue->freq_switch)/2.;
}
/**
  * @brief multiply of all losses values
  * @param fet struct ptr
  * @retval total losses
  */
double SwMosfet::swMosfetTotalLoss(PMosfet *pmvalue)
{
    return (pmvalue->mosfet_conduct_loss)*(pmvalue->mosfet_drive_loss)*(pmvalue->mosfet_switch_loss)*(pmvalue->mosfet_capacit_loss);
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
double SwMosfet::swMosfetCustomIdrv(InputValue *ivalue)
{
    return Qg*ivalue->freq_switch;
}
/**
  * @brief get current value
  * @param input struct ptr
  * @retval continuous gate current
  */
double SwMosfet::getCustomIdrv(InputValue *ivalue)
{
    return swMosfetCustomIdrv(ivalue);
}
