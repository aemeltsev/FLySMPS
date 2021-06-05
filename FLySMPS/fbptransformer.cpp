/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

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
 * @brief EnergyStoredChoke - The maximum energy stored in the inductor
 * @return stored value in watt(W_l) w
 */
double FBPTCore::EnergyStoredChoke() const
{
    return (primary_induct * qPow(curr_primary_peak, 2))/2.;
}

/**
 * @brief CoreAreaProd - Estimate the core area product or Ap. The window
 *                       utilization factor(K_u) and the saturation flux
 *                       density(B_sat) are assumed a priori. The current
 *                       density of the conductor is selected based on the
 *                       peak current requement.
 * @return core area value(A_p) m^4 - using for select next core parameters
 */
double FBPTCore::CoreAreaProd() const
{
    return (2. * EnergyStoredChoke())/(m_ca.win_util_factor * m_ca.max_curr_dens * m_ca.mag_flux_dens);
}

/**
 * @brief CoreGeometryCoeff - The core geometry coefficient. The K_g provides
 *                            the core with a good combination of W_a, A_c and
 *                            MLT satisfying electromagnetic condition, dc-winding-loss
 *                            condition, and core-area restriction simultaneously.
 * @param outPow - Total output power in converter
 * @return core geometry coefficient(K_g) m^5
 */
double FBPTCore::CoreGeometryCoeff(int16_t outPwr) const
{
    return (2 * S_RO_OM * primary_induct * EnergyStoredChoke() * qPow(curr_primary_rms,2))/(outPwr * qPow(m_ca.mag_flux_dens,2));
}

/**
 * @brief CoreWinToCoreSect - Estimate cross-sectional area to Window area core(WaAe) equivalent to Ap
 * @return core area value(A_p) m^4
 */
double FBPTCore::CoreAreaProd_WaAe() const
{
    double tmp = (primary_induct * curr_primary_rms * curr_primary_peak)/(m_ca.mag_flux_dens * S_K_1);
    return qPow(tmp, (4./3.));
}

/**
 * @brief AreaWindTotal - The cross-sectional area of the winding bare wire
 * @param cs - Multiparameters object, contain core selection properties
 * @return cross-section wind area(A_w) m^2
 */
double FBPTCore::AreaWindTotal(const CoreSelection &cs) const
{
    double wa=0., result=0.;

    if(cs.core_wind_area){
        result = m_ca.win_util_factor * cs.core_wind_area * S_RO_OM * cs.mean_leng_per_turn;
    }
    else{
        wa = CoreAreaProd()/cs.core_cross_sect_area;
        result = m_ca.win_util_factor * wa * S_RO_OM * cs.mean_leng_per_turn;
    }
    return curr_primary_peak * qSqrt(result/static_cast<double>(power_out_max));
}

/**
 * @brief CurrentDens - Current density of the winding
 * @param cs - Multiparameters object, contain core selection properties
 * @return - current density(J_m) A/m^2
 */
double FBPTCore::CurrentDens(const CoreSelection &cs) const
{
    return curr_primary_peak/AreaWindTotal(cs);
}
/*Area product calculation*/

/**
  * @brief numPrimary - Estimate turns of primary side
  * @param cs - Multiparameters object, contain core selection properties
  * @param fns - Select method for num primary turns calculate
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
        temp = (primary_induct*curr_primary_peak_peak)/(cs.core_cross_sect_area * m_ca.mag_flux_dens);
    }
    else if(fns == FBPT_NUM_SETTING::FBPT_CORE_AREA)
    {
        auto check_wa = [&](){
            if(cs.core_wind_area){
                return cs.core_wind_area;
            }
            else{
                return CoreAreaProd()/cs.core_cross_sect_area;
            }
        };
        temp = (m_ca.win_util_factor * check_wa())/AreaWindTotal(cs);
    }
    return temp;
}

/*Air gap methods*/
/**
  * @brief agLength - Air gap length
  * @param cs - Multiparameters object, contain core selection properties
  * @param varNumPrim - Number of turns the primary side
  * @return Air gap length(l_g) m
  */
double FBPTCore::agLength(const CoreSelection &cs, double varNumPrim) const
{
    return ((S_MU_Z * cs.core_cross_sect_area * std::pow(varNumPrim, 2))/(primary_induct))-
            (cs.mean_mag_path_leng / cs.core_permeal);
}

 /**
 * @brief agFringFluxFact - A friging flux  is present around the air gap
 *                          whenever the core is excited. The magnetic flux
 *                          lines bulge outward because the magnetic lines
 *                          repel each other when passing through a nonmagnetic
 *                          material. As result, the cross-sectional area
 *                          of the magnetic field is increased and the flux
 *                          density is decreased this effect is called the
 *                          fringing flux effect.
 * @param cs - Multiparameters object, contain core selection properties
 * @param varNumPrim - Number of turns the primary side
 * @param fsag - Select core central kern shape
 * @param mchdm - Mechanical dimensions of the core
 * @return fringing flux factor value()
 */
double FBPTCore::agFringFluxFact(const CoreSelection &cs, double varNumPrim, /*double ewff,*/
                                 FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm) const
{
    double csa, af, temp = 0.0;
    double k = /*empl/agLength(cs, varNumPrim);*/ 2;//empl - the mean effective of the magnetic path length in the fringing area
    double u = /*ewff/agLength(cs, varNumPrim);*/ 1;//ewff - the effective width of the fringing flux cross-sectional area
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
 * @brief actNumPrimary - Actual number of turns to the primary side
 * @param cs - Multiparameters object, contain core selection properties
 * @param fsag - Select core central kern shape
 * @param mchdm - Mechanical dimensions of the core
 * @param varNumPrim - The last calculated Number of turns
 * @param varIndPrim - Primary inductance
 * @param currPeakPrim - Primary peak current
 * @return actual number of turns value
 */
int16_t FBPTCore::actNumPrimary(const CoreSelection &cs, FBPT_SHAPE_AIR_GAP &fsag,
                                MechDimension &mchdm, double varNumPrim,
                                double varIndPrim, double currPeakPrim) const
{
    int16_t act_num_prim_turns = 0;
    double ag, ffg, flux_peak = 0.0;
    do
    {
        ag = agLength(cs, varNumPrim);
        ffg = agFringFluxFact(cs, varNumPrim, fsag, mchdm);
        act_num_prim_turns = static_cast<int16_t>(qSqrt((ag*varIndPrim)/(S_MU_Z*cs.core_cross_sect_area*ffg)));
        flux_peak = (S_MU_Z * act_num_prim_turns * ffg * (currPeakPrim/2))/(ag+(cs.mean_mag_path_leng/cs.core_permeal));
    }
    while(flux_peak > m_ca.mag_flux_dens);
    return act_num_prim_turns;
}

/**
 * @brief actMagneticFluxPeak - Actual magnetic flux peak
 *                              More see Sekiya H.- Design of RF-choke inductors using core geometry coefficient
 * @param cs - Multiparameters object, contain core selection properties
 * @param actNumPrim - Actual number of turns to the primary side
 * @param maxCurPrim - Primary peak current
 * @param agLength - Air gap length
 * @return actual maximum flux density
 */
double FBPTCore::actMagneticFluxPeak(const CoreSelection &cs, int16_t actNumPrim, double maxCurPrim, float agLength) const
{
    return (S_MU_Z * actNumPrim * maxCurPrim)/(agLength+(cs.mean_mag_path_leng/cs.core_permeal));
}

/**
 * @brief actDutyCycle - Recalculate actual value for duty cycle using 4 output voltage/current values
 *                       More see Ayachit A.-Magnetising inductance of multiple-output flyback dc–dc convertor for dcm
 * @param outVtcr - Output secondary voltage/current vector
 * @param in_volt_min - Input minimal DC voltage
 * @param fsw - Switching frequency
 * @param prim_ind - Primary inductance
 * @return duty cycle value
 */
float FBPTCore::actDutyCycle(QVector<QPair<float, float>> outVtcr, int16_t in_volt_min,
                             int16_t fsw, double prim_ind) const
{
    auto get_max = [](double frst, double scnd){return qMax(frst, scnd);};

    auto max_vdc1_ratio = [&outVtcr, &in_volt_min](){return outVtcr[0].first/in_volt_min;}; //first - n-th voltage out
    auto duty1_max = max_vdc1_ratio()*qSqrt((2*fsw*prim_ind)/(outVtcr[0].first/outVtcr[0].second));

    auto max_vdc2_ratio = [&outVtcr, &in_volt_min](){return outVtcr[1].first/in_volt_min;};
    auto duty2_max = max_vdc2_ratio()*qSqrt((2*fsw*prim_ind)/(outVtcr[1].first/outVtcr[1].second));

    auto max_vdc3_ratio = [&outVtcr, &in_volt_min](){return outVtcr[2].first/in_volt_min;};
    auto duty3_max = max_vdc3_ratio()*qSqrt((2*fsw*prim_ind)/(outVtcr[2].first/outVtcr[2].second));

    auto max_vdc4_ratio = [&outVtcr, &in_volt_min](){return outVtcr[3].first/in_volt_min;};
    auto duty4_max = max_vdc4_ratio()*qSqrt((2*fsw*prim_ind)/(outVtcr[3].first/outVtcr[3].second));

    return static_cast<float>(qMax(get_max(duty1_max, duty2_max), get_max(duty3_max, duty4_max)));
}

/**
 * @brief actReflVoltage - The recalculate actual value of the reflected voltage
 *                         use a duty cycle of the secondary side - (1-D)
 *                         More see AN5287 170W high input voltage two switch flyback based on L6565 and 1500V K5 MOSFETs
 * @param actDuty - Actual duty cycle value
 * @param maxOutPwr - Common output power value
 * @param primInduct - Primary inductance
 * @param fsw - Switching frequency value
 * @return reflected voltage value
 */
int16_t FBPTCore::actReflVoltage(float actDuty, float maxOutPwr,
                                 double primInduct, int16_t fsw) const
{
    return qSqrt(2*maxOutPwr*primInduct*fsw)/(1-actDuty);
}

/*Recalc Np, Bm */

/*Second Side*/
/**
 * @brief outNumSecond -
 * @return -
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
inline double FBPTSecondary::outCurrRMSSecond()
{
    double tmp = ((1-actual_duty_cycle)/actual_duty_cycle);
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
