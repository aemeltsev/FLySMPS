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
    return qPow((input_dc_min_voltage * DutyCycleDCM()), 2)/(2. * InputPower()*static_cast<double>(freq_switch)*ripple_factor);
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
    return qSqrt((3.*(qPow(CurrPriAver(),2))+(qPow((CurrPriPeakToPeak()/2.),2)))*(DutyCycleDCM()/3.));
}

/*All current primary side*/

/*Area product calculation*/
/**
 * @brief EnergyStoredChoke -
 * @return in W
 */
double FBPTCore::EnergyStoredChoke() const
{
    return (primary_induct * qPow(curr_primary_peak, 2))/2.;
}

/**
 * @brief CoreAreaProd - Estimate the core geometry coefficient Kg or Ap
 * @return
 */
double FBPTCore::CoreAreaProd() const
{
    return (2. * EnergyStoredChoke())/(curr_dens * core_win_util_fact * flux_dens_max);
}

/**
  * @brief
  * @param
  * @return
  */
/*
double FBPTCore::DeltaFluxMax()
{
    return  flux_dens_max * (curr_primary_peak_peak/(1.1*(curr_primary_peak_peak)));
}
*/

/**
 * @brief CoreWinToCoreSect - Calculate windows area product AP
 * @return area product ratio
 */
double FBPTCore::CoreWinToCoreSect() const
{
    double tmp = (primary_induct * curr_primary_rms * curr_primary_peak)/(flux_dens_max * S_K_1);
    return qPow(tmp, (4./3.));
}

/**
 * @brief AreaWindTotal
 * @param cs
 * @return
 */
double FBPTCore::AreaWindTotal(const CoreSelection &cs) const
{
    double tmp = core_win_util_fact * cs.core_wind_area * S_RO_OM * cs.mean_leng_per_turn;
    return curr_primary_peak * qSqrt(tmp/static_cast<double>(power_out_max));
}

/**
 * @brief CurrentDens
 * @param cs
 * @return
 */
double FBPTCore::CurrentDens(const CoreSelection &cs) const
{
    return curr_primary_peak/AreaWindTotal(cs);
}
/*Area product calculation*/

/*Primary turns*/
/**
  * @brief numPrimary - Estimate turns of primary side
  * @param cs - core geometry values
  * @param fns - select method for num primary turns calculate
  * @return number of turns the primary side
  */
double FBPTCore::numPrimary(const CoreSelection &cs, const FBPT_NUM_SETTING &fns)
{
    double temp = 0.0;
    if(fns == FBPT_NUM_SETTING::FBPT_INDUCT_FACTOR)
    {
        temp = qSqrt(primary_induct/cs.ind_fact);
    }
    else if(fns == FBPT_NUM_SETTING::FBPT_FLUX_PEAK)
    {
        temp = (primary_induct*curr_primary_peak_peak)/(cs.core_cross_sect_area * flux_dens_max);
    }
    else if(fns == FBPT_NUM_SETTING::FBPT_CORE_AREA)
    {
        temp = (core_win_util_fact * cs.core_wind_area)/AreaWindTotal(cs);
    }
    return temp;
}
/*Primary turns*/

/*Air gap methods*/
/**
  * @brief agLength - Air gap length
  * @param cs - core parameters
  * @param varNumPrim - number of turns the primary side
  * @return length ag in m
  */
double FBPTCore::agLength(const CoreSelection &cs, double varNumPrim) const
{
    return ((S_MU_Z * cs.core_cross_sect_area * std::pow(varNumPrim, 2))/(primary_induct))-
            (cs.mean_mag_path_leng / cs.core_permeal);
}

 /**
 * @brief agFringFluxFact -
 * @param cs
 * @param varNumPrim
 * @param ewff
 * @param fsag
 * @param mchdm
 * @param k
 * @return
 */
double FBPTCore::agFringFluxFact(const CoreSelection &cs, double varNumPrim, double ewff,
                                 FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm, double k) const
{
    double csa, af, temp = 0.0;
    double u = ewff/agLength(cs, varNumPrim);
    if(fsag == FBPT_SHAPE_AIR_GAP::RECT_AIR_GAP)
    {
        csa = mchdm.C*mchdm.D;
        af = 2. * u * agLength(cs, varNumPrim) * (mchdm.C + mchdm.D + 2. * u * agLength(cs, varNumPrim));
        temp = 1 + (af/(csa*k));
    }
    else if(fsag == FBPT_SHAPE_AIR_GAP::ROUND_AIR_GAP)
    {
        csa = (M_PI*qPow(mchdm.Diam, 2))/4.;
        af = M_PI * u * agLength(cs, varNumPrim) * (mchdm.C + mchdm.D + 2. * u * agLength(cs, varNumPrim));
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
int16_t FBPTCore::actNumPrimary(const CoreSelection &cs, FBPT_SHAPE_AIR_GAP &fsag,
                                MechDimension &mchdm, double ewff,
                                double varNumPrim, double varIndPrim,
                                double currPeakPrim, double k=1.0) const
{
    int16_t act_num_prim_turns = 0;
    double ag, ffg, flux_peak = 0.0;
    do
    {
        ag = agLength(cs, varNumPrim);
        ffg = agFringFluxFact(cs, varNumPrim, ewff, fsag, mchdm, k);
        act_num_prim_turns = static_cast<int16_t>(qSqrt((ag*varIndPrim)/(S_MU_Z*cs.core_cross_sect_area*ffg)));
        flux_peak = (S_MU_Z * act_num_prim_turns * ffg * (currPeakPrim/2))/(ag+(cs.mean_mag_path_leng/cs.core_permeal));
    }
    while(flux_peak > flux_dens_max);
    return act_num_prim_turns;
}
/*Recalc Np, Bm */

/*Recalc actual methods vreflected and duty*/
/**
 * @brief postVoltageRefl - Post calculated reflected voltage
 * @param actual_num_primary - num primary turns
 * @param voltout - output voltage
 * @param voltdrop - diode drop voltage
 * @param numsec - secondary turns controlled side
 * @return actual voltage reflected in V
 */
inline double postVoltageRefl(int16_t actual_num_primary, float voltout, float voltdrop, float numsec)
{
    return static_cast<double>((voltout + voltdrop))*(actual_num_primary/static_cast<double>(numsec));
}

/**
 * @brief postMaxDutyCycle - Post calculated max duty
 * @param actual_volt_reflected - post calculated reflected voltage
 * @param input_min_voltage - recalculation after input capacitor selection
 * @return actual duty cucle ratio
 */
inline double postMaxDutyCycle(int16_t actual_volt_reflected, int16_t input_min_voltage)
{
    return (actual_volt_reflected)/(actual_volt_reflected + input_min_voltage);
}
/*Recalc actual methods vreflected and duty*/

/*Second Side*/
/**
 * @brief outNumSecond -
 * @return
 */
double FBPTSecondary::outNumSecond() const
{
    return static_cast<double>(actual_num_primary * (volt + diode_drop_sec))/static_cast<double>(refl_volt);
}

/**
 * @brief outNumTurnRatio -
 * @return
 */
double FBPTSecondary::outNumTurnRatio() const
{
    return actual_num_primary/outNumSecond();
}

/**
 * @brief outCurrPeakSecond -
 * @return
 */
inline double FBPTSecondary::outCurrPeakSecond()
{
    return (curr_primary_peak) * outNumTurnRatio() * outCoeffPWR();
}

/**
 * @brief outCurrRMSSecond -
 * @param actual_volt_reflected - post calculated reflected voltage
 * @param input_min_voltage - recalculation after input capacitor selection
 * @return
 */
inline double FBPTSecondary::outCurrRMSSecond(int16_t actual_volt_reflected, int16_t input_min_voltage)
{
    double tmp = ((1-postMaxDutyCycle(actual_volt_reflected, input_min_voltage))/postMaxDutyCycle(actual_volt_reflected, input_min_voltage));
    return curr_primary_rms * outCoeffPWR() * outNumTurnRatio() * qSqrt(tmp);
}
/*Second Side*/

/*Winding*/
/**
 * @brief wEffBobbWidth -
 * @param mchdm -
 * @return
 */
inline double FBPTWinding::wEffBobbWidth(const MechDimension &mchdm) const {return mchdm.D-(2. * M);}

/**
 * @brief wEffWindCrossSect -
 * @param cs
 * @param mchdm
 * @return
 */
inline double FBPTWinding::wEffWindCrossSect(const CoreSelection &cs, const MechDimension &mchdm) const {return (cs.core_wind_area*(wEffBobbWidth(mchdm)))/mchdm.D;}

/**
 * @brief wCoperWireCrossSectArea
 * @param cs
 * @param mchdm
 * @param windfact
 * @return
 */
inline double FBPTWinding::wCoperWireCrossSectArea(const CoreSelection &cs, const MechDimension &mchdm, double windfact) const
{
    return (windfact * FCu *(wEffWindCrossSect(cs, mchdm)))/actual_num_primary;
}

/**
 * @brief wMaxWireSizeAWG -
 * @param wirecrosssect
 * @return
 */
inline double FBPTWinding::wMaxWireSizeAWG(double wirecrosssect) const
{
    return (9.97*(1.8277 - (2*std::log10(2*qSqrt((wirecrosssect)/M_PI)))));
}

/**
 * @brief wSkinDepth -
 * @return
 */
inline double FBPTWinding::wSkinDepth() const
{
    return qSqrt((S_RO_OM)/(2.*M_PI*static_cast<double>(freq_switch)*S_MU_Z));
}

/**
 * @brief setWireDiam -
 * @param awgp
 * @param np
 */
void FBPTWinding::setWireDiam(double awgp, uint16_t np)
{
    AWGp = awgp;
    Np = np;
}

/**
 * @brief wCoperWireDiam -
 * @return
 */
inline double FBPTWinding::wCoperWireDiam() const
{
    double tmp = (AWGp)/(2.*9.97);
    double out = ((1.8277/2.)-(tmp));
    return qPow(10., out);
}

/**
 * @brief wCoperWireCrossSectAreaPost -
 * @return
 */
inline double FBPTWinding::wCoperWireCrossSectAreaPost() const
{
    return (M_PI/4.)*qPow(wCoperWireDiam(), 2.)*Np;
}

/**
 * @brief wCurrentDenst -
 * @return
 */
inline double FBPTWinding::wCurrentDenst() const
{
    return static_cast<double>(curr_primary_rms)/wCoperWireCrossSectAreaPost();
}

/**
 * @brief wNumTurnToLay -
 * @param mchdm
 * @return
 */
inline double FBPTWinding::wNumTurnToLay(const MechDimension &mchdm) const
{
    return (wEffBobbWidth(mchdm))/(Np*(wCoperWireDiam()+(2*INS)));
}

/**
 * @brief wNumLay -
 * @param mchdm
 * @return
 */
inline double FBPTWinding::wNumLay(const MechDimension &mchdm) const
{
    return (actual_num_primary)/(wNumTurnToLay(mchdm));
}
/*Winding*/
