/**
  * License text...
*/
#include "flytransformer.h"

FlyTransformer::FlyTransformer()
{

}


/*Air gap methods*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::agLength(FBTransformer &fbtvalue, double &varNumPrim)
{
    return ((S_MU_Z*core_cross_sect_area*pow(varNumPrim, 2))/(fbtvalue.primary_induct))-(mean_mag_path_leng/core_permeal);
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
double FlyTransformer::agFringArea(FBTransformer &fbtvalue)
{
    return (2.*1.*(fbtvalue.length_air_gap))*(C + F +(2.*1.*(fbtvalue.length_air_gap)));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::agFringFluxFact(FBTransformer &fbtvalue)
{
    return 1. + (fbtvalue.fring_area/core_cross_sect_area);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actNumPrimary(FBTransformer &fbtvalue)
{
    return sqrt(((fbtvalue.primary_induct)/S_MU_Z*core_cross_sect_area)*(fbtvalue.length_air_gap + (mean_mag_path_leng/core_permeal)));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actFluxDensPeak(FBTransformer &fbtvalue)
{
    return ((S_MU_Z*fbtvalue.actual_num_primary)/(fbtvalue.length_air_gap + (mean_mag_path_leng/core_permeal)))*(fbtvalue.curr_primary_peak + (fbtvalue.curr_primary_peak_peak/2.));
}
/*Air gap methods*/
/**/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numOutPower(double &OutCurr, double &OutVolt)
{
    return (OutCurr)*(OutVolt);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numCoeffPower(InputValue &ivalue, double &OutPower)
{
    return OutPower/ivalue.power_out_max;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numSecondary(FBTransformer &fbtvalue, InputValue &ivalue, double &OutVolt)
{
    return (fbtvalue.actual_num_primary*(OutVolt)*(ivalue.volt_diode_drop_sec))/fbtvalue.actual_volt_reflected;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::numTurnsRatio(FBTransformer &fbtvalue, double &NumTurns)
{
    return fbtvalue.actual_num_primary/(NumTurns);
}
/**/
/*Recalc actual methods vreflected and duty*/
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actVoltageRefl(InputValue &ivalue, FBTransformer &fbtvalue, double &varNumSec)
{
    return (ivalue.volt_out_one + ivalue.volt_diode_drop_sec)*(fbtvalue.actual_num_primary/(varNumSec));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::actMaxDutyCycle(FBTransformer &fbtvalue, BCap &bcvalue)
{
    return (fbtvalue.actual_volt_reflected)/(fbtvalue.actual_volt_reflected + bcvalue.input_min_voltage);
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
double FlyTransformer::wEffWindCrossSect()
{
    return (core_wind_area*(wEffBobbWidth()))/D;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCoperWireCrossSectArea(FBTransformer &fbtvalue, double &WindFact)
{
    return ((WindFact)*FCu*(wEffWindCrossSect()))/(fbtvalue.actual_num_primary);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wMaxWireSizeAWG(double &WireCrossSect)
{
    return (9.97*(1.8277 - (2*log10(2*sqrt((WireCrossSect)/S_PI)))));
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
double FlyTransformer::wCoperWireDiam(double &WireSizeAWG)
{
    double snafu = (WireSizeAWG)/(2.*9.97);
    double fubar = ((1.8277/2.)-(snafu));
    return pow(10., fubar);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCoperWireCrossSectAreaPost(double &WireDiam)
{
    return (S_PI/4.)*pow(WireDiam, 2.)*Np;
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wCurrentDenst(FBTransformer &fbtvalue, double &WireAreaPost)
{
    return (fbtvalue.curr_primary_rms)/(WireAreaPost);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wNumTurnToLay(double &WireDiam)
{
    return (wEffBobbWidth())/(Np*((WireDiam)+(2*INS)));
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wNumLay(FBTransformer &fbtvalue, double &NumTurPerLay)
{
    return (fbtvalue.actual_num_primary)/(NumTurPerLay);
}
//Current for secondary layers
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wSecondCurrPeak(FBTransformer &fbtvalue, double &TurnRatio, double &CoeffPwr)
{
    return  (fbtvalue.curr_primary_peak)*(TurnRatio)*(CoeffPwr);
}
/**
  * @brief
  * @param
  * @retval
  */
double FlyTransformer::wSecondCurrRMS(FBTransformer &fbtvalue, double &CoeffPwr, double &TurnRatio)
{
    double fubar = ((1-fbtvalue.actual_max_duty_cycle)/fbtvalue.actual_max_duty_cycle);
    return fbtvalue.curr_primary_rms*(CoeffPwr)*(TurnRatio)*sqrt(fubar);
}
/*Winding*/
