#include "swmosfet.h"

SwMosfet::SwMosfet()
{

}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetVoltageNom(DBridge *dbvalue, FBTransformer *fbtvalue)
{
    return dbvalue->in_max_rms_voltage + fbtvalue->actual_volt_reflected;
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetVoltageMax(PMosfet *pmvalue, InputValue *ivalue)
{
    return pmvalue->mosfet_voltage_nom + ivalue->voltage_spike;
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetCurrent(FBTransformer *fbtvalue, InputValue *ivalue, DBridge *dbvalue)
{
    double leakage = ivalue->leakage_induct * fbtvalue->primary_induct;
    double time_switch = (1/ivalue->freq_switch);
    return (ivalue->power_out_max/(ivalue->eff * dbvalue->in_min_rms_voltage * fbtvalue->actual_max_duty_cycle)) + ((dbvalue->in_min_rms_voltage * fbtvalue->actual_max_duty_cycle * time_switch)/leakage);
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetRiseTime()
{
    return Qg*(2./Idrv);
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetConductLoss(FBTransformer *fbtvalue)
{
    return pow(fbtvalue->curr_primary_rms, 2) * RDSon;
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetDriveLoss(InputValue *ivalue)
{
    return Vdrv*Qg*ivalue->freq_switch;
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetSwitchLoss(InputValue *ivalue, FBTransformer *fbtvalue, PMosfet *pmvalue)
{
    return ((Coss*pow(pmvalue->mosfet_voltage_max, 2)*ivalue->freq_switch)/2.) + (pmvalue->mosfet_voltage_max*fbtvalue->curr_primary_peak*pmvalue->mosfet_rise_time*ivalue->freq_switch);
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetCapacitLoss(PMosfet *pmvalue, InputValue *ivalue)
{
    return (Coss*pow(pmvalue->mosfet_voltage_max, 2)*ivalue->freq_switch)/2.;
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetTotalLoss(PMosfet *pmvalue)
{
    return (pmvalue->mosfet_conduct_loss)*(pmvalue->mosfet_drive_loss)*(pmvalue->mosfet_switch_loss)*(pmvalue->mosfet_capacit_loss);
}
/**
  * @brief
  * @param
  * @retval
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
  * @brief
  * @param
  * @retval
  */
double SwMosfet::swMosfetCustomIdrv(InputValue *ivalue)
{
    return Qg*ivalue->freq_switch;
}
/**
  * @brief
  * @param
  * @retval
  */
double SwMosfet::getCustomIdrv(InputValue *ivalue)
{
    return swMosfetCustomIdrv(ivalue);
}
