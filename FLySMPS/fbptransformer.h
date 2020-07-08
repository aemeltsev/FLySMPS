#ifndef FBPTRANSFORMER_H
#define FBPTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class FBPTPrimary
{
public:
    FBPTPrimary(double krf): ripple_factor(krf){}

    double DutyCycleDCM(const InputValue &ivalue, const BCap &bcvalue);
    double InputPower(const InputValue &ivalue);
    double PriInduct(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue);

    /*All current primary side*/
    double CurrPriAver(const BCap &bcvalue, const FBPT &fbptval);
    double CurrPriPeakToPeak(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue);
    double CurrPriMax(const FBPT &fbptval);
    double CurrPriValley(const FBPT &fbptval);
    double CurrPriRMS(const FBPT &fbptval);
    /*All current primary side*/
private:
    double ripple_factor;
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
private:
    double curr_dens;//Jm - the maximum current density
    double core_win_util_fact;//Ku - window utilization factor
    double flux_dens_max;//Bm - saturation magnetic field density

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

    double EnergyStoredChoke(const FBPT &fbptval);//
    //Correction factor F. - the edge coefficient(FFC)
    double agFringFluxFact(const FBPT &fbptval, double ewff,
                           FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                           double k=1.0);

public:
    /*Core Geometry Factor and Core Selection*/
    void setCoreSelection(double ap, double mu_rc,
                          double ac, double wa,
                          double vc, double lt,
                          double lc, double hw,
                          double al, CoreSelection &cs);
    double CoreAreaProd(const FBPT &fbptval);//Core geometry coefficient(Ap)
    double CoreWinToCoreSect(const FBPT &fbptval);//Cross-sectional area to Window area core(WaAe)
    double DeltaFluxMax(const FBPT &fbptval);
    double AreaWindTotal(const FBPT &fbptval, const InputValue &ivalue, const CoreSelection &cs);//Cross-sectional area of the winding bare wire
    double CurrentDens(const FBPT &fbptval);
    /*Core Geometry Factor and Core Selection*/

    double numPrimary(const FBPT &fbptval, const CoreSelection &cs, const FBPT_NUM_SETTING &fns);

    /*Air-Gap Length Considered with Fringing Effect*/
    void setMechanDimension(double f, double c,
                            double e, double d,
                            MechDimension &mch, double diam);
    double agLength(const FBPT &fbptval, const CoreSelection &cs, double varNumPrim);//The air-gap length(lg)
    /*Air-Gap Length Considered with Fringing Effect*/

    /*Recalc Np, Bm */
    int16_t actNumPrimary(const FBPT &fbptval, const CoreSelection &cs,
                          double varNumPrim, double ewff,
                          FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                          double k);
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
