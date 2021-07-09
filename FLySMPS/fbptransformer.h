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

#ifndef FBPTRANSFORMER_H
#define FBPTRANSFORMER_H
#include <QtMath>
#include <QVector>
#include <QPair>
#include <cstdint>

#define S_MU_Z     4.*M_PI*1E-7 //H/m
#define S_RO_OM    1.72E-8 //Ohm/m
#define S_K_1      85*1E-4

class FBPTPrimary
{
private:
    double ripple_factor;
    int16_t refl_volt;
    double pow_max_out;
    float efficiency;
    float freq_switch;

    int16_t input_dc_min_voltage; // dc average, between min input and rectify min peak
    int16_t input_min_voltage; // recalc after input capacitor selection

public:
    /**
     * @brief FBPTPrimary
     * @param krf
     * @param rv
     * @param pout
     * @param eff
     * @param swfr
     */
    FBPTPrimary(double krf, int16_t rv,
                double pout, float eff,
                float swfr):
        ripple_factor(krf), refl_volt(rv),
        pow_max_out(pout), efficiency(eff),
        freq_switch(swfr)
    {}

    /**
     * @brief setInputVoltage
     * @param idcmv - dc average, between min input and rectify min peak
     * @param imv - recalc after input capacitor selection
     */
    void setInputVoltage(int16_t input_volt_ac_min,
                         double input_pwr,
                         int16_t freq_line,
                         double bulk_cap_value,
                         double bulk_cap_delta_time)
    {
        double input_pk_min_voltage = input_volt_ac_min * M_SQRT2;
        double pwr_cap_coeff = input_pwr / bulk_cap_value;
        double chg_time = (1 / freq_line) - 2 * bulk_cap_delta_time;
        input_min_voltage = qSqrt(qPow(input_pk_min_voltage, 2) - (pwr_cap_coeff * chg_time));
        input_dc_min_voltage = 0.5 * (input_pk_min_voltage + input_min_voltage);
    }

    /*Inductance of primary side*/
    /**
     * @brief DutyCycleDCM - Maximum duty cycle
     * @return duty cycle ratio
     */
    double DutyCycleDCM()
    {
        return static_cast<double>(refl_volt)/(refl_volt+input_dc_min_voltage);
    }

    /**
     * @brief InputPower - Maximum input power
     * @return in pwr in W
     */
    double InputPower()
    {
        return pow_max_out/static_cast<double>(efficiency);
    }

    /**
     * @brief PriInduct - Primary inductance
     * @return inductance in H
     */
    double PriInduct()
    {
        return qPow((input_dc_min_voltage * DutyCycleDCM()), 2)/(2. * InputPower()*static_cast<double>(freq_switch)*ripple_factor);
    }
    /*Inductance of primary side*/

    /** All current primary side*/
    /**
     * @brief CurrPriAver - Primary average current during turn-on
     * @return average current
     */
    double CurrPriAver()
    {
        return InputPower()/(input_min_voltage*DutyCycleDCM());
    }

    /**
     * @brief CurrPriPeakToPeak - Primary peak-to-peak current
     * @return primare delta current
     */
    double CurrPriPeakToPeak()
    {
        return (input_dc_min_voltage*DutyCycleDCM())/(PriInduct()*static_cast<double>(freq_switch));
    }

    /**
      * @brief CurrPriMax - Primary peak current
      * @return peak value of primary current
      */
    double CurrPriMax()
    {
        return  CurrPriAver()+(CurrPriPeakToPeak()/2);
    }

    /**
      * @brief CurrPriValley - Primary valley current
      * @return the valley of the inductor current
      */
    double CurrPriValley()
    {
        return CurrPriMax()-CurrPriPeakToPeak();
    }

    /**
      * @brief CurrPriRMS - Primary RMS current
      * @return current rms value
      */
    double CurrPriRMS()
    {
        return qSqrt((3.*(qPow(CurrPriAver(),2))+(qPow((CurrPriPeakToPeak()/2.),2)))*(DutyCycleDCM()/3.));
    }

    /*All current primary side*/
};

struct CoreArea
{
    double mag_flux_dens;
    double win_util_factor;
    int16_t max_curr_dens;
};

struct CoreSelection
{
    //double core_area_product;//Ap
    //double core_win_height;//height of the window
    double ind_fact;//Al(inductance factor TDK)
    /**< Core parameters */
    double core_cross_sect_area;//Ac(Ae - effective magnetic cross section TDK)
    double core_wind_area;//Wa(An - winding cross section TDK)
    double core_vol;//Vc(Ve - effective magnetic volume TDK)
    double mean_leng_per_turn;//l_t(l_n - average length of turn TDK)
    double mean_mag_path_leng;//l_c(l_e - effective magnetic path length TDK)
    double core_permeal;//mu_rc(mu_r - relative permeability TDK)
};

struct MechDimension
{
    /* Rectangular Air Gap */
    float C; /**< c - width of core, TDK dimension nomenclature */
    float E; /**< e - window width, TDK dimension nomenclature */
    float F; /**< f - window half height, TDK dimension nomenclature */
    float D; /**< d - size of rectangular central kern, TDK dimension nomenclature */
    /* Round Air Gap */
    float Diam; /**< diam - size of round central kern, TDK dimension nomenclature */
};

enum class FBPT_NUM_SETTING
{
    FBPT_INDUCT_FACTOR = 0,
    FBPT_FLUX_PEAK     = 1,
    FBPT_CORE_AREA     = 2
};

enum class FBPT_SHAPE_AIR_GAP
{
    RECT_AIR_GAP  = 0,
    ROUND_AIR_GAP = 1
};

class FBPTCore
{
public:
    /**
     * @brief FBPTCore
     * @param currdens
     * @param utilfact
     * @param fluxdens
     */
    FBPTCore(CoreArea& ca, double prin,
             double pkprcr, double rmsprcr,
             double ppprcr, double pout)
        :primary_induct(prin)//Lp - primary inductance
        ,curr_primary_peak(pkprcr)//Ippk - primary peak current
        ,curr_primary_rms(rmsprcr)//Iprms - primary RMS current
        ,curr_primary_peak_peak(ppprcr)//Ippkpk -
        ,power_out_max(pout)//Pout - max output power
    {
        //Jm - the maximum current density
        //Ku - window utilization factor
        //Bm - saturation magnetic field density
        qSwap(m_ca, ca);
    }

private:
    CoreArea m_ca;
    double primary_induct;
    double curr_primary_peak;
    double curr_primary_rms;
    double curr_primary_peak_peak;
    double power_out_max;

    /**
     * @brief EnergyStoredChoke - The maximum energy stored in the inductor
     * @return stored value in watt(W_l) w
     */
    double EnergyStoredChoke() const
    {
        return (primary_induct * qPow(curr_primary_peak, 2))/2.;
    }

public:
    /**
     * @brief CoreAreaProd - Estimate the core area product or Ap. The window
     *                       utilization factor(K_u) and the saturation flux
     *                       density(B_sat) are assumed a priori. The current
     *                       density of the conductor is selected based on the
     *                       peak current requement.
     * @return core area value(A_p) m^4 - using for select next core parameters
     */
    double CoreAreaProd() const
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
    double CoreGeometryCoeff(double outPwr) const
    {
        double k_electr = outPwr * qPow(m_ca.mag_flux_dens, 2);
        return (2. * 1.72 * qPow(EnergyStoredChoke(), 2)) / (k_electr * 0.5);
        //return (2 * S_RO_OM * primary_induct * EnergyStoredChoke() * qPow(curr_primary_rms,2))/(outPwr * qPow(m_ca.mag_flux_dens,2));
    }

    /**
     * @brief CoreWinToCoreSect - Estimate cross-sectional area to Window area core(WaAe) equivalent to Ap
     * @return core area value(A_p) m^4
     */
    double CoreAreaProd_WaAe() const
    {
        double tmp = (primary_induct * curr_primary_rms * curr_primary_peak)/(m_ca.mag_flux_dens * S_K_1);
        return qPow(tmp, (4./3.));
    }

private:
    /**
     * @brief AreaWindTotal - The cross-sectional area of the winding bare wire
     * @param cs - Multiparameters object, contain core selection properties
     * @return cross-section wind area(A_w) m^2
     */
    double AreaWindTotal(const CoreSelection &cs) const
    {
        double wa=0., result=0.;
    //FIX Branch
        if(cs.core_wind_area != -1.0){
            result = m_ca.win_util_factor * cs.core_wind_area * S_RO_OM * cs.mean_leng_per_turn;
        }
        else{
            wa = CoreAreaProd()/cs.core_cross_sect_area;
            result = m_ca.win_util_factor * wa * S_RO_OM * cs.mean_leng_per_turn;
        }
        //A_w - in A/m^2 to A/mm^2 - A_w*10^-6
        return curr_primary_peak * qSqrt(result/static_cast<double>(power_out_max));
    }

public:

    /**
     * @brief CurrentDens - Current density of the winding
     * @param cs - Multiparameters object, contain core selection properties
     * @return - current density(J_m) A/m^2
     */
    double CurrentDens(const CoreSelection &cs) const
    {
        //J_m - in A/m^2 to A/mm^2 - J_m*10^-6
        return curr_primary_peak/AreaWindTotal(cs);
    }

    /**
      * @brief numPrimary - Estimate turns of primary side
      * @param cs - Multiparameters object, contain core selection properties
      * @param fns - Select method for num primary turns calculate
      * @return number of turns the primary side
      */
    double numPrimary(const CoreSelection &cs, const FBPT_NUM_SETTING &fns)
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
                if(cs.core_wind_area != -1.0){
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

    /**
      * @brief agLength - Air gap length
      * @param cs - Multiparameters object, contain core selection properties
      * @param varNumPrim - Number of turns the primary side
      * @return Air gap length(l_g)
      */
    double agLength(const CoreSelection &cs, double varNumPrim) const
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
   double agFringFluxFact(const CoreSelection &cs, double varNumPrim, /*double ewff,*/
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

   /*Recalc Np, Bm, Duty, Vro */
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
   int16_t actNumPrimary(const CoreSelection &cs, FBPT_SHAPE_AIR_GAP &fsag,
                                   MechDimension &mchdm, uint32_t varNumPrim,
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
   double actMagneticFluxPeak(const CoreSelection &cs, uint32_t actNumPrim, double maxCurPrim, double agLength) const
   {
       return (S_MU_Z * actNumPrim * maxCurPrim)/(agLength + (cs.mean_mag_path_leng/cs.core_permeal));
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
   float actDutyCycle(const QVector<QPair<float, float>>& outVtcr, double in_volt_min,
                                int32_t fsw, double prim_ind) const
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
   int16_t actReflVoltage(float actDuty, float maxOutPwr,
                                    double primInduct, int32_t fsw) const
   {
       return qSqrt(2*maxOutPwr*primInduct*fsw)/(1-actDuty);
   }
};

class FBPTSecondary
{
public:
    /**
     * @brief FBPTSecondary - ctor for implement one secondary side winding
     * @param currout - out current
     * @param voltout - out voltage
     * @param voltreflect - implement reflected voltage
     * @param powout - common maximum power output
     * @param aprnm - actual number turns of the primary side
     * @param adc - actual duty cycle
     * @param ddrp - secondary side diode voltage drop
     */
    FBPTSecondary(float currout, float voltout,
                  float voltreflect, float powout,
                  int16_t aprnm, float adc, float ddrp):
        curr(currout), volt(voltout),
        refl_volt(voltreflect), pow_out_max(powout),
        actual_num_primary(aprnm), actual_duty_cycle(adc), diode_drop_sec(ddrp)
    {}

    inline double outPWR() const
    {
        return static_cast<double>(curr*volt);
    }

    inline double outCoeffPWR() const
    {
        return outPWR()/ static_cast<double>(pow_out_max);
    }

    /**
     * @brief outNumSecond -
     * @return -
     */
    double outNumSecond() const
    {
        return static_cast<double>(actual_num_primary * (volt + diode_drop_sec))/static_cast<double>(refl_volt);
    }

    /**
     * @brief outNumTurnRatio -
     * @return
     */
    double outNumTurnRatio() const
    {
        return actual_num_primary/outNumSecond();
    }

    void setCurrentParam(double pkcpr, double rmscpr)
    {
        curr_primary_peak = pkcpr; curr_primary_rms = rmscpr;
    }

    /**
     * @brief outCurrPeakSecond - Peak current(JSP)
     * @return
     */
    inline double outCurrPeakSecond()
    {
        return (curr_primary_peak) * outNumTurnRatio() * outCoeffPWR();
    }

    /**
     * @brief outCurrRMSSecond - RMS current(JSRMS)
     * @param actual_volt_reflected - post calculated reflected voltage
     * @param input_min_voltage - recalculation after input capacitor selection
     * @return
     */
    inline double outCurrRMSSecond()
    {
        double tmp = (1 - static_cast<double>(actual_duty_cycle)) / static_cast<double>(actual_duty_cycle);
        return curr_primary_rms * outCoeffPWR() * outNumTurnRatio() * qSqrt(tmp);
    }
private:
    float curr;
    float volt;
    float refl_volt;
    float pow_out_max;
    int16_t actual_num_primary;
    float actual_duty_cycle;
    float diode_drop_sec;

    double curr_primary_peak;
    double curr_primary_rms;
};

class FBPTWinding
{
public:
    /**
     * @brief FBPTWinding
     * @param aprnm
     * @param frsw
     * @param rmscp
     * @param m
     * @param fcu
     * @param ins
     */
    FBPTWinding(float frsw, float m=4., double fcu=0.4, double ins=0.01)
        :freq_switch(frsw)
        ,M(m)
        ,FCu(fcu)
        ,INS(ins)
    {}

    /*Winding*/
    /**
     * @brief wEffBobbWidth - Effective bobbin width(BWe)
     * @param mchdm -
     * @return
     */
    inline double wEffBobbWidth(const MechDimension &mchdm) const
    {
        return static_cast<double>(mchdm.E) - (2. * static_cast<double>(M));
    }

    /**
     * @brief wEffWindCrossSect - Effective winding cross-section(ANe)
     * @param cs
     * @param mchdm
     * @return
     */
    inline double wEffWindCrossSect(const CoreSelection &cs, const MechDimension &mchdm) const
    {
        return (cs.core_wind_area*(wEffBobbWidth(mchdm))) / static_cast<double>(mchdm.E);
    }

    /**
     * @brief wCoperWireCrossSectArea - (AP) or (ANS)
     * @param cs
     * @param mchdm
     * @param windfact
     * @return
     */
    inline double wCoperWireCrossSectArea(const CoreSelection &cs, const MechDimension &mchdm, double windfact, uint32_t num_turns) const
    {
        return (windfact * FCu *(wEffWindCrossSect(cs, mchdm))) / num_turns;
    }

    /**
     * @brief wMaxWireSizeAWG - (AWGP) or (AWGNS)
     * @param wirecrosssect
     * @return
     */
    inline double wMaxWireSizeAWG(double wirecrosssect) const
    {
        wirecrosssect *= 1E+6;
        return (9.97 * (1.8277 - (2 * std::log10(2 * qSqrt(wirecrosssect / M_PI)))));
    }

    /**
     * @brief wSkinDepth -
     * @return
     */
    inline double wSkinDepth() const
    {
        return qSqrt((S_RO_OM) / (2. * M_PI * static_cast<double>(freq_switch)*S_MU_Z));
    }

    /**
     * @brief setWireDiam -
     * @param awgp
     */
    void setWireDiam(float awgp){ AWGp = awgp;}

    /**
     * @brief wCoperWireDiam - (DP) or (DS)
     * @return mm
     */
    inline double wCoperWireDiam() const
    {
        double tmp = static_cast<double>(AWGp)/(2.*9.97);
        double out = ((1.8277/2.)-(tmp));
        return qPow(10., out);
    }

    /**
     * @brief wCoperWireCrossSectAreaPost - (ECA) - Effective copper area
     * @param mchdm
     * @return in mm^2
     */
    inline double wCoperWireCrossSectAreaPost(int16_t npw) const
    {
        return qPow((wCoperWireDiam() / 2.), 2.) * M_PI * npw;
    }

    /**
     * @brief wCurrentDenst - (JP) or (JS)
     * @return in A/mm^2
     */
    inline double wCurrentDenst(double curr_rms, int16_t npw) const
    {
        return static_cast<double>(curr_rms)/wCoperWireCrossSectAreaPost(npw);
    }

    /**
     * @brief wOuterDiam - (OD) - Wire outer diameter including insulation
     * @return in mm
     */
    inline double wOuterDiam() const
    {
        return wCoperWireDiam() + (2 * INS);
    }

    /**
     * @brief wNumTurnToLay - Number of turns per layer(NTL)
     * @param mchdm
     * @return in turns/layer
     */
    inline double wNumTurnToLay(const MechDimension &mchdm, int16_t npw) const
    {
        return (wEffBobbWidth(mchdm) / (wOuterDiam() * npw)) * 1000;
    }

    /**
     * @brief wNumLay - (LN)
     * @param mchdm
     * @return number of layer value
     */
    inline double wNumLay(const MechDimension &mchdm, uint32_t num_turns, int16_t npw) const
    {
        return num_turns / wNumTurnToLay(mchdm, npw);
    }

    /*Winding*/
private:
    float freq_switch;
    float M;
    double FCu;
    float AWGp;
    double INS;
};
#endif // FBPTRANSFORMER_H
