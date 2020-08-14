#ifndef FBPTRANSFORMER_H
#define FBPTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class FBPTPrimary
{
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
                int16_t pout, float eff,
                float swfr):
        ripple_factor(krf), refl_volt(rv),
        pow_max_out(pout), efficiency(eff),
        freq_switch(swfr)
    {}

    double DutyCycleDCM();
    double InputPower();
    double PriInduct();

    /**
     * @brief setInputVoltage
     * @param idcmv - dc average, between min input and rectify min peak
     * @param imv - recalc after input capacitor selection
     */
    void setInputVoltage(int16_t idcmv, int16_t imv){input_dc_min_voltage = idcmv; input_min_voltage = imv;}
    /*All current primary side*/
    double CurrPriAver();
    double CurrPriPeakToPeak();
    double CurrPriMax();
    double CurrPriValley();
    double CurrPriRMS();
    /*All current primary side*/
private:
    double ripple_factor;
    int16_t refl_volt;
    int16_t pow_max_out;
    float efficiency;
    float freq_switch;

    int16_t input_dc_min_voltage; // dc average, between min input and rectify min peak
    int16_t input_min_voltage; // recalc after input capacitor selection
};

struct CoreSelection
{
    double core_area_product;//Ap
    double core_permeal;//mu_rc(mu_r - relative permeability TDK)
    double core_cross_sect_area;//Ac(Ae - effective magnetic cross section TDK)
    double core_wind_area;//Wa(An - winding cross section TDK)
    double core_vol;//Vc(Ve - effective magnetic volume TDK)
    double mean_leng_per_turn;//l_t(l_n - average length of turn TDK)
    double mean_mag_path_leng;//l_c(l_e - effective magnetic path length TDK)
    double core_win_height;//height of the window
    double ind_fact;//Al(inductance factor TDK)
};

struct MechDimension
{
    /* Rectangular Air Gap */
    double C;
    double E;
    double F;
    double D; //size of rectangulared, central kern
    /* Round Air Gap */
    double Diam;
};

class FBPTCore
{
public:
    FBPTCore(double currdens, double utilfact, double fluxdens):
        curr_dens(currdens),
        core_win_util_fact(utilfact),
        flux_dens_max(fluxdens)
    {}

    /**
     * @brief setPreCalc
     * @param prin
     * @param pkprcr
     * @param rmsprcr
     */
    void setPreCalc(double prin, double pkprcr,
                    double rmsprcr, float pout,
                    double ppprcr)
    {
        primary_induct = prin;
        curr_primary_peak = pkprcr;
        curr_primary_rms = rmsprcr;
        power_out_max = pout;
        curr_primary_peak_peak = ppprcr;
    }
private:
    double curr_dens;//Jm - the maximum current density
    double core_win_util_fact;//Ku - window utilization factor
    double flux_dens_max;//Bm - saturation magnetic field density
    double primary_induct;//Lp
    double curr_primary_peak;//Ippk
    double curr_primary_rms;//Iprms
    double curr_primary_peak_peak;//Ippkpk
    float power_out_max;

    enum FBPT_NUM_SETTING
    {
        FBPT_INDUCT_FACTOR,
        FBPT_FLUX_PEAK,
        FBPT_CORE_AREA,
    };
    enum FBPT_SHAPE_AIR_GAP
    {
        RECT_AIR_GAP,
        ROUND_AIR_GAP,
    };

    inline double EnergyStoredChoke() const;//
    //Correction factor F. - the edge coefficient(FFC)
    double agFringFluxFact(const CoreSelection &cs, double varNumPrim, double ewff,
                           FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                           double k=1.0);

public:
    /*Core Geometry Factor and Core Selection*/
    void setCoreSelection(double ap, double mu_rc,
                          double ac, double wa,
                          double vc, double lt,
                          double lc, double hw,
                          double al, CoreSelection &cs);
    inline double CoreAreaProd() const;//Core geometry coefficient(Ap)
    inline double CoreWinToCoreSect() const;//Cross-sectional area to Window area core(WaAe)
    //inline double DeltaFluxMax() const;
    inline double AreaWindTotal(const CoreSelection &cs) const;//Cross-sectional area of the winding bare wire
    inline double CurrentDens(const CoreSelection &cs) const;
    /*Core Geometry Factor and Core Selection*/

    double numPrimary(const CoreSelection &cs, const FBPT_NUM_SETTING &fns);

    /*Air-Gap Length Considered with Fringing Effect*/
    void setMechanDimension(double f, double c,
                            double e, double d,
                            MechDimension &mch, double diam);
    inline double agLength(const CoreSelection &cs, double varNumPrim) const;//The air-gap length(lg)
    /*Air-Gap Length Considered with Fringing Effect*/

    /*Recalc Np, Bm */
    inline int16_t actNumPrimary(const FBPT &fbptval, const CoreSelection &cs,
                          double varNumPrim, double ewff,
                          FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                          double k) const;
    /*Recalc Np, Bm */
};

inline double postVoltageRefl(const InputValue &ivalue, const FBPT &fbptval, double &varNumSec);//Post-calculated reflected voltage(VRPost)
inline double postMaxDutyCycle(const FBPT &fbptval, const BCap &bcvalue);//Post-calculated maximum duty cycle(DMaxPost)

class FBPTSecondary
{
public:
    FBPTSecondary(double currout, double voltout, double voltreflect):
        Curr(currout), Volt(voltout), ReflVolt(voltreflect)
    {}
    inline double outPWR(){return Curr*Volt;}
    inline double outCoeffPWR(const InputValue &ivalue){return outPWR()/ivalue.power_out_max;}
    inline double outNumSecond(const FBPT &fbptval, const InputValue &ivalue);
    inline double outNumTurnRatio(const FBPT &fbptval, const InputValue &ivalue);

    //Current for secondary layers
    inline double outCurrPeakSecond(const FBPT &fbptval, const InputValue &ivalue);//Peak current(IAMax)
    inline double outCurrRMSSecond(const FBPT &fbptval, const BCap &bcvalue, const InputValue &ivalue);//RMS current(ISRMS)
private:
    double Curr;
    double Volt;
    double ReflVolt;
};

class FBPTWinding
{
public:
    FBPTWinding(int8_t m=4, double fcu=0.4, double ins=0.01):
        M(m), FCu(fcu), INS(ins)
    {

    }
    /*Winding*/
    inline double wEffBobbWidth(const MechDimension &mchdm);//Effective bobbin width(BWe)
    inline double wEffWindCrossSect(const CoreSelection &cs, const MechDimension &mchdm);//Effective winding cross-section(ANe)
    inline double wCoperWireCrossSectArea(const FBPT &fbptval, const CoreSelection &cs, const MechDimension &mchdm, double windfact);//(AP) or (ANS)
    inline double wMaxWireSizeAWG(double wirecrosssect);//(AWGP) or (AWGNS)
    inline double wSkinDepth(const InputValue &ivalue);
    void setWireDiam(double awgp, uint16_t np);
    inline double wCoperWireDiam();//(DP) or (DS)
    inline double wCoperWireCrossSectAreaPost();//
    inline double wCurrentDenst(const FBPT &fbptval);//(JP) or (JS)
    inline double wNumTurnToLay(const MechDimension &mchdm);//Number of turns per layer(NL)
    inline double wNumLay(const FBPT &fbptval, const MechDimension &mchdm);//(LNp)
    /*Winding*/
private:
    int8_t M;
    double FCu;
    double AWGp;
    double Np;
    double INS;
};
#endif // FBPTRANSFORMER_H
