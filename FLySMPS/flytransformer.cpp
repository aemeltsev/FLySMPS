#include "flytransformer.h"

FlyTransformer::FlyTransformer()
{

}
/*Inductance of primary side*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::DutyCycleDCM(InputValue *ivalue, BCap *bcvalue)
{
    return ivalue->refl_volt_max/((ivalue->refl_volt_max)+(bcvalue->input_dc_min_voltage));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::InputPower(InputValue *ivalue)
{
    return (ivalue->power_out_max)/(ivalue->eff);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::PriInduct(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue)
{
    return ((bcvalue->input_dc_min_voltage*fbtvalue->max_duty_cycle)*(bcvalue->input_dc_min_voltage*fbtvalue->max_duty_cycle))/(2.*fbtvalue->inp_power*ivalue->freq_switch*KRF);
}
/*Inductance of primary side*/
/*All current primary side*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriAver(BCap *bcvalue, FBTransformer *fbtvalue)
{
    return fbtvalue->inp_power/((bcvalue->input_min_voltage)*(fbtvalue->max_duty_cycle));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriPeakToPeak(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue)
{
    return ((bcvalue->input_dc_min_voltage)*(fbtvalue->max_duty_cycle))/((fbtvalue->primary_induct)*(ivalue->freq_switch));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriMax(FBTransformer *fbtvalue)
{
    return  (fbtvalue->curr_primary_aver)+((fbtvalue->curr_primary_peak_peak)/2);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriValley(FBTransformer *fbtvalue)
{
    return (fbtvalue->curr_primary_peak)-(fbtvalue->curr_primary_peak_peak);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrPriRMS(FBTransformer *fbtvalue)
{
    return sqrt((3.*(fbtvalue->curr_primary_aver*fbtvalue->curr_primary_aver)+((fbtvalue->curr_primary_peak_peak/2.)*(fbtvalue->curr_primary_peak_peak/2.)))*(fbtvalue->max_duty_cycle/3.));
}
/*All current primary side*/
/*Area product calculation*/
/**
  * @brief
  * @param
  * @retval
  */
void FlyTransformer::setMagneteValues(double currdens, double utilfact, double fluxdens, double krf)
{
    currdens = curr_dens;
    utilfact = core_win_util_fact;
    fluxdens = flux_dens_max;
    krf = KRF;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::EnergyStoredChoke(FBTransformer *fbtvalue)
{
    return (fbtvalue->primary_induct*((fbtvalue->curr_primary_peak)*(fbtvalue->curr_primary_peak)))/2.;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CoreAreaProd(FBTransformer *fbtvalue)
{
    return (2.*(fbtvalue->energy_stored_choke))/(curr_dens*core_win_util_fact*flux_dens_max);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::DeltaFluxMax(FBTransformer *fbtvalue)
{
    return  flux_dens_max*(fbtvalue->curr_primary_peak_peak/(1.1*fbtvalue->curr_primary_peak));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CoreWinToCoreSect(FBTransformer *fbtvalue)
{
    double fubar = (fbtvalue->primary_induct*fbtvalue->curr_primary_rms*fbtvalue->curr_primary_peak)/(flux_dens_max*S_K_1);
    return pow(fubar, (4./3.));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::AreaWindTotal(FBTransformer *fbtvalue, InputValue *ivalue)
{
    return sqrt((core_win_util_fact*core_wind_area*S_RO_OM*mean_leng_per_turn*pow(fbtvalue->curr_primary_peak,2))/ivalue->power_out_max);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::CurrentDens(FBTransformer *fbtvalue)
{
    return fbtvalue->curr_primary_peak/fbtvalue->area_wind_tot;
}
/**
  * @brief
  * @param
  * @retval
  */
void FlyTransformer::setCoreSelection(double ap, double mu_rc, double ac, double wa, double vc, double lt, double lc, double hw, double al)
{
    ap = core_area_product;
    mu_rc = core_permeal;
    ac = core_cross_sect_area;
    wa = core_wind_area;
    vc = core_vol;
    lt = mean_leng_per_turn;
    lc = mean_mag_path_leng;
    hw = core_win_height;
    al = ind_fact;
}
/*Area product calculation*/
/*Primary turns*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numPimaryAL(FBTransformer *fbtvalue)
{
    return sqrt((fbtvalue->primary_induct)/ind_fact);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numPimaryBA(FBTransformer *fbtvalue)
{
    return ((fbtvalue->primary_induct)*(fbtvalue->curr_primary_peak_peak))/(flux_dens_max*core_cross_sect_area);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numPrimaryWireArea(FBTransformer *fbtvalue)
{
    return (core_win_util_fact*core_wind_area)/fbtvalue->area_wind_tot;
}
/*Primary turns*/
/*Air gap methods*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::agLength(FBTransformer *fbtvalue, double *varNumPrim)
{
    return ((S_MU_Z*core_cross_sect_area*pow(*varNumPrim, 2))/(fbtvalue->primary_induct))-(mean_mag_path_leng/core_permeal);
}
/**
  * @brief
  * @param
  * @retval
  */
void FlyTransformer::setMechanDimension(double c, double f, double e, double d)
{
    C = c;
    F = f;
    E = e;
    D = d;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::agFringArea(FBTransformer *fbtvalue)
{
    return (2.*1.*(fbtvalue->length_air_gap))*(C + F +(2.*1.*(fbtvalue->length_air_gap)));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::agFringFluxFact(FBTransformer *fbtvalue)
{
    return 1. + (fbtvalue->fring_area/core_cross_sect_area);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actNumPrimary(FBTransformer *fbtvalue)
{
    return sqrt(((fbtvalue->primary_induct)/S_MU_Z*core_cross_sect_area)*(fbtvalue->length_air_gap + (mean_mag_path_leng/core_permeal)));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actFluxDensPeak(FBTransformer *fbtvalue)
{
    return ((S_MU_Z*fbtvalue->actual_num_primary)/(fbtvalue->length_air_gap + (mean_mag_path_leng/core_permeal)))*(fbtvalue->curr_primary_peak + (fbtvalue->curr_primary_peak_peak/2.));
}
/*Air gap methods*/
/**/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numOutPower(double *OutCurr, double *OutVolt)
{
    return (*OutCurr)*(*OutVolt);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numCoeffPower(InputValue *ivalue, double *OutPower)
{
    return *OutPower/ivalue->power_out_max;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numSecondary(FBTransformer *fbtvalue, InputValue *ivalue, double *OutVolt)
{
    return (fbtvalue->actual_num_primary*(*OutVolt)*ivalue->volt_diode_drop_sec)/fbtvalue->actual_volt_reflected;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numTurnsRatio(FBTransformer *fbtvalue, double *NumTurns)
{
    return fbtvalue->actual_num_primary/(*NumTurns);
}
/**/
/*Recalc actual methods vreflected and duty*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actVoltageRefl(InputValue *ivalue, FBTransformer *fbtvalue, double *varNumSec)
{
    return (ivalue->volt_out_one + ivalue->volt_diode_drop_sec)*(fbtvalue->actual_num_primary/(*varNumSec));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actMaxDutyCycle(FBTransformer *fbtvalue, BCap *bcvalue)
{
    return fbtvalue->actual_volt_reflected/(fbtvalue->actual_volt_reflected + bcvalue->input_min_voltage);
}
/*Recalc actual methods vreflected and duty*/
/*Winding*/
/**
  * @brief
  * @param
  * @retval
  */
void FlyTransformer::setWindVal(double m, double fcu)
{
    M = m;
    FCu = fcu;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wEffBobbWidth()
{
    return D - (2. * M);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wEffWindCrossSect(FBTransformer *fbtvalue)
{
    return (core_wind_area*fbtvalue->eff_bobb_width)/D;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCoperWireCrossSectArea(FBTransformer *fbtvalue, double *WindFact)
{
    return ((*WindFact)*FCu*fbtvalue->eff_wind_cross_sect)/(fbtvalue->actual_num_primary);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wMaxWireSizeAWG(double *WireCrossSect)
{
    return (9.97*(1.8277 - (2*log10(2*sqrt((*WireCrossSect)/S_PI)))));
}
/**
  * @brief
  * @param
  * @retval
  */
void FlyTransformer::setWireDiam(double awgp, uint16_t np, double ins)
{
    AWGp = awgp;
    Np = np;
    INS = ins;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCoperWireDiam(double *WireSizeAWG)
{
    double snafu = (*WireSizeAWG)/(2.*9.97);
    double fubar = ((1.8277/2.)-(snafu));
    return pow(10., fubar);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCoperWireCrossSectAreaPost(double *WireDiam)
{
    return (S_PI/4.)*pow(*WireDiam, 2.)*Np;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCurrentDenst(FBTransformer *fbtvalue, double *WireAreaPost)
{
    return fbtvalue->curr_primary_rms/(*WireAreaPost);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wNumTurnToLay(FBTransformer *fbtvalue, double *WireDiam)
{
    return fbtvalue->eff_bobb_width/(Np*((*WireDiam)+(2*INS)));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wNumLay(FBTransformer *fbtvalue, double *NumTurPerLay)
{
    return fbtvalue->actual_num_primary/(*NumTurPerLay);
}
//Current for secondary layers
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wSecondCurrPeak(FBTransformer *fbtvalue, double *TurnRatio, double *CoeffPwr)
{
    return  fbtvalue->curr_primary_peak*(*TurnRatio)*(*CoeffPwr);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wSecondCurrRMS(FBTransformer *fbtvalue, double *CoeffPwr, double *TurnRatio)
{
    double fubar = ((1-fbtvalue->actual_max_duty_cycle)/fbtvalue->actual_max_duty_cycle);
    return fbtvalue->curr_primary_rms*(*CoeffPwr)*(*TurnRatio)*sqrt(fubar);
}
/*Winding*/
