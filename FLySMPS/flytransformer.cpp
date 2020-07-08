/**
  * License text...
*/
#include "flytransformer.h"

FlyTransformer::FlyTransformer()
{

}

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
