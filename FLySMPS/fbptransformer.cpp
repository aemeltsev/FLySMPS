#include "fbptransformer.h"

/*Inductance of primary side*/
/**
 * @brief DutyCycleDCM - Maximum duty cycle
 * @return duty cycle ratio
 */
double FBPTPrimary::DutyCycleDCM()
{
    return  refl_volt/(refl_volt+input_dc_min_voltage);
}

/**
 * @brief InputPower - Maximum input power
 * @return in pwr in W
 */
double FBPTPrimary::InputPower()
{
    return pow_max_out/static_cast<double>(efficiency);
}

/**
 * @brief PriInduct - Primary inductance
 * @return inductance in H
 */
double FBPTPrimary::PriInduct()
{
    return std::pow((input_dc_min_voltage * DutyCycleDCM()), 2)/(2. * InputPower()*static_cast<double>(freq_switch)*ripple_factor);
}
/*Inductance of primary side*/

/** All current primary side*/
/**
 * @brief CurrPriAver - Primary average current during turn-on
 * @return average current
 */
double FBPTPrimary::CurrPriAver()
{
    return InputPower()/(input_min_voltage*DutyCycleDCM());
}

/**
 * @brief CurrPriPeakToPeak - Primary peak-to-peak current
 * @return primare delta current
 */
double FBPTPrimary::CurrPriPeakToPeak()
{
    return (input_dc_min_voltage*DutyCycleDCM())/(PriInduct()*static_cast<double>(freq_switch));
}

/**
  * @brief CurrPriMax - Primary peak current
  * @return peak value of primary current
  */
double FBPTPrimary::CurrPriMax()
{
    return  CurrPriAver()+(CurrPriPeakToPeak()/2);
}

/**
  * @brief CurrPriValley - Primary valley current
  * @return the valley of the inductor current
  */
double FBPTPrimary::CurrPriValley()
{
    return CurrPriMax()-CurrPriPeakToPeak();
}

/**
  * @brief CurrPriRMS - Primary RMS current
  * @return current rms value
  */
double FBPTPrimary::CurrPriRMS()
{
    return std::sqrt((3.*(std::pow(CurrPriAver(),2))+(std::pow((CurrPriPeakToPeak()/2.),2)))*(DutyCycleDCM()/3.));
}

/*All current primary side*/

/*Area product calculation*/
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::EnergyStoredChoke(const FBPT &fbptval)
{
    return (fbptval.primary_induct * pow(fbptval.curr_primary_peak, 2))/2.;
}
/**
  * @brief Estimate the core geometry coefficient
  * @param
  * @return
  */
double FBPTCore::CoreAreaProd(const FBPT &fbptval)
{
    return (2. * EnergyStoredChoke(fbptval))/(curr_dens * core_win_util_fact * flux_dens_max);
}
/**
  * @brief
  * @param
  * @return
  */
/*
double FBPTCore::DeltaFluxMax(const FBPT &fbptval)
{
    return  flux_dens_max * (fbptval.curr_primary_peak_peak/(1.1*(fbptval.curr_primary_peak_peak)));
}
*/
/**
  * @brief Calculate windows area product AP
  * @param use from FPBT struct - primary inductance, rms and peak current
  * @return area product ratio
  */
double FBPTCore::CoreWinToCoreSect(const FBPT &fbptval)
{
    double tmp = (fbptval.primary_induct * fbptval.curr_primary_rms * fbptval.curr_primary_peak)/(flux_dens_max * S_K_1);
    return pow(tmp, (4./3.));
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::AreaWindTotal(const FBPT &fbptval, const InputValue &ivalue, const CoreSelection &cs)
{
    double tmp = core_win_util_fact * cs.core_wind_area * S_RO_OM * cs.mean_leng_per_turn;
    return fbptval.curr_primary_peak * sqrt(tmp/ivalue.power_out_max);
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::CurrentDens(const FBPT &fbptval)
{
    return fbptval.curr_primary_peak/fbptval.area_wind_tot;
}
/**
  * @brief
  * @param
  * @return
  */
void FBPTCore::setCoreSelection(double ap, double mu_rc,
                                double ac, double wa,
                                double vc, double lt,
                                double lc, double hw, double al,
                                CoreSelection &cs)
{
 cs.core_area_product = ap;
 cs.core_permeal = mu_rc;
 cs.core_cross_sect_area = ac;
 cs.core_wind_area = wa;
 cs.core_vol = vc;
 cs.mean_leng_per_turn = lt;
 cs.mean_mag_path_leng = lc;
 cs.core_win_height = hw;
 cs.ind_fact = al;
}
/*Area product calculation*/

/*Primary turns*/
/**
  * @brief Estimate turns of primary side
  * @param from FBPT inductance and p-p current
  * @param cs - core geometry values
  * @param fns - select method for num primary turns calculate
  * @return number of turns the primary side
  */
double FBPTCore::numPrimary(const FBPT &fbptval, const CoreSelection &cs, const FBPT_NUM_SETTING &fns)
{
    double temp = 0.0;
    if(fns == FBPT_INDUCT_FACTOR)
    {
        temp = sqrt((fbptval.primary_induct)/cs.ind_fact);
    }
    else if(fns == FBPT_FLUX_PEAK)
    {
        temp = ((fbptval.primary_induct)*(fbptval.curr_primary_peak_peak))/(cs.core_cross_sect_area * flux_dens_max);
    }
    else if(fns == FBPT_CORE_AREA)
    {
        temp = (core_win_util_fact * cs.core_wind_area)/fbptval.area_wind_tot;
    }
    return temp;
}
/*Primary turns*/

/*Air gap methods*/
/**
  * @brief Mechanical dimension
  * @param c - width of core, TDK dimension nomenclature
  * @param e - window width, TDK dimension nomenclature
  * @param f - window half height, TDK dimension nomenclature
  * @param d - size of rectangular central kern, TDK dimension nomenclature
  * @param diam - size of round central kern, TDK dimension nomenclature
  */
void FBPTCore::setMechanDimension(double c, double e,
                                  double f, double d, MechDimension &mchdm, double diam=0.0)
{
    mchdm.C = c;
    mchdm.E = e;
    mchdm.F = f;
    mchdm.D = d;
    mchdm.Diam = diam;
}
/**
  * @brief Air gap length
  * @param from fbptval - primary inductance
  * @param cs - core parameters
  * @param varNumPrim - number of turns the primary side
  * @return length ag in m
  */
double FBPTCore::agLength(const FBPT &fbptval, const CoreSelection &cs, double varNumPrim)
{
    return ((S_MU_Z * cs.core_cross_sect_area * pow(varNumPrim, 2))/(fbptval.primary_induct))-
            (cs.mean_mag_path_leng / cs.core_permeal);
}
/**
  * @brief
  * @param
  * @return
  */
double FBPTCore::agFringFluxFact(const FBPT &fbptval, double ewff,
                                 FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm, double k)
{
    double csa, af, temp = 0.0;
    double u = ewff/fbptval.length_air_gap;
    if(fsag == RECT_AIR_GAP)
    {
        csa = mchdm.C*mchdm.D;
        af = 2. * u * fbptval.length_air_gap * (mchdm.C + mchdm.D + 2. * u * fbptval.length_air_gap);
        temp = 1 + (af/(csa*k));
    }
    else if(fsag == ROUND_AIR_GAP)
    {
        csa = (S_PI*pow(mchdm.Diam, 2))/4.;
        af = S_PI * u * fbptval.length_air_gap * (mchdm.C + mchdm.D + 2. * u * fbptval.length_air_gap);
        temp = 1 + (af/(csa*k));
    }
    return temp;
}
/*Recalc Np, Bm */
/**
  * @brief
  * @param
  * @return
  */
int16_t FBPTCore::actNumPrimary(const FBPT &fbptval, const CoreSelection &cs,
                                double varNumPrim, double ewff,
                                FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                                double k=1.0)
{
    int16_t act_num_prim_turns = 0;
    double ag, ffg, flux_peak = 0.0;
    do
    {
        ag = agLength(fbptval, cs, varNumPrim);
        ffg = agFringFluxFact(fbptval, ewff, fsag, mchdm, k);
        act_num_prim_turns = static_cast<int16_t>(sqrt((ag*fbptval.primary_induct)/(S_MU_Z*cs.core_cross_sect_area*ffg)));
        flux_peak = (S_MU_Z * act_num_prim_turns * ffg * (fbptval.curr_primary_peak/2))/(ag+(cs.mean_mag_path_leng/cs.core_permeal));
    }
    while(flux_peak > flux_dens_max);
    return act_num_prim_turns;
}
/*Recalc Np, Bm */

/*Recalc actual methods vreflected and duty*/
/**
  * @brief
  */
inline double postVoltageRefl(const FBPT &fbptval, float voltout, float voltdrop, float numsec)
{
    return static_cast<double>((voltout + voltdrop))*(fbptval.actual_num_primary/static_cast<double>(numsec));
}
/**
  * @brief
  */
inline double postMaxDutyCycle(const FBPT &fbptval, const BCap &bcvalue)
{
    return (fbptval.actual_volt_reflected)/(fbptval.actual_volt_reflected + bcvalue.input_min_voltage);
}
/*Recalc actual methods vreflected and duty*/

/*Second Side*/
/**
  * @brief
  */
double FBPTSecondary::outNumSecond(const FBPT &fbptval, const InputValue &ivalue)
{
    return (fbptval.actual_num_primary * (Volt + ivalue.volt_diode_drop_sec))/ReflVolt;
}
/**
  * @brief
  */
double FBPTSecondary::outNumTurnRatio(const FBPT &fbptval, const InputValue &ivalue)
{
    return fbptval.actual_num_primary/outNumSecond(fbptval, ivalue);
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTSecondary::outCurrPeakSecond(const FBPT &fbptval, const InputValue &ivalue)
{
    return (fbptval.curr_primary_peak) * outNumTurnRatio(fbptval, ivalue) * outCoeffPWR(ivalue);
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTSecondary::outCurrRMSSecond(const FBPT &fbptval, const BCap &bcvalue, const InputValue &ivalue)
{
    double tmp = ((1-postMaxDutyCycle(fbptval, bcvalue))/postMaxDutyCycle(fbptval, bcvalue));
    return fbptval.curr_primary_rms * outCoeffPWR(ivalue) * outNumTurnRatio(fbptval, ivalue) *sqrt(tmp);
}
/*Second Side*/

/*Winding*/
/**
  * @param
  * @return
  */
inline double FBPTWinding::wEffBobbWidth(const MechDimension &mchdm){return mchdm.D-(2. * M);}
/**
  * @param
  * @return
  */
inline double FBPTWinding::wEffWindCrossSect(const CoreSelection &cs, const MechDimension &mchdm){return (cs.core_wind_area*(wEffBobbWidth(mchdm)))/mchdm.D;}
/**
  * @param
  * @return
  */
inline double FBPTWinding::wCoperWireCrossSectArea(const FBPT &fbptval, const CoreSelection &cs, const MechDimension &mchdm, double windfact)
{
    return (windfact * FCu *(wEffWindCrossSect(cs, mchdm)))/(fbptval.actual_num_primary);
}
/**
  * @param
  * @return
  */
inline double FBPTWinding::wMaxWireSizeAWG(double wirecrosssect)
{
    return (9.97*(1.8277 - (2*log10(2*sqrt((wirecrosssect)/S_PI)))));
}
/**
  * @param
  * @return
  */
inline double FBPTWinding::wSkinDepth(const InputValue &ivalue)
{
    return sqrt((S_RO_OM)/(2.*S_PI*ivalue.freq_switch*S_MU_Z));
}
/**
  * @brief
  * @param
  */
void FBPTWinding::setWireDiam(double awgp, uint16_t np)
{
    AWGp = awgp;
    Np = np;
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTWinding::wCoperWireDiam()
{
    double tmp = (AWGp)/(2.*9.97);
    double out = ((1.8277/2.)-(tmp));
    return pow(10., out);
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTWinding::wCoperWireCrossSectAreaPost()
{
    return (S_PI/4.)*pow(wCoperWireDiam(), 2.)*Np;
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTWinding::wCurrentDenst(const FBPT &fbptval)
{
    return fbptval.curr_primary_rms/wCoperWireCrossSectAreaPost();
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTWinding::wNumTurnToLay(const MechDimension &mchdm)
{
    return (wEffBobbWidth(mchdm))/(Np*(wCoperWireDiam()+(2*INS)));
}
/**
  * @brief
  * @param
  * @retval
  */
inline double FBPTWinding::wNumLay(const FBPT &fbptval, const MechDimension &mchdm)
{
    return (fbptval.actual_num_primary)/(wNumTurnToLay(mchdm));
}
/*Winding*/
