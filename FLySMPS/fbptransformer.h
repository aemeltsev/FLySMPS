#ifndef FBPTRANSFORMER_H
#define FBPTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class FBPTransformer
{
public:
    FBPTransformer(double krf): ripple_factor(krf){}

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

    double EnergyStoredChoke(const FBPT &fbptval);//
    double agFringFluxFact(const FBPT &fbptval, double ewff,
                           FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                           double k=1.0);//Correction factor F. - the edge coefficient(FFC)

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

class FBPTWinding
{
public:
    FBPTWinding();

    double actVoltageRefl(const InputValue &ivalue, const FBPT &fbptval, double &varNumSec);//Post-calculated reflected voltage(VRPost)
    double actMaxDutyCycle(const FBPT &fbptval, const BCap &bcvalue);//Post-calculated maximum duty cycle(DMaxPost)

    /*Winding*/
    void setWindVal(double m, double fcu);
    double wEffBobbWidth();//Effective bobbin width(BWe)
    double wEffWindCrossSect();//Effective winding cross-section(ANe)
    double wCoperWireCrossSectArea(FBTransformer &fbtvalue,  double &WindFact);//(AP) or (ANS)
    double wMaxWireSizeAWG(double &WireCrossSect);//(AWGP) or (AWGNS)
    void setWireDiam(double awgp, uint16_t np, double ins);
    double wCoperWireDiam(double &WireSizeAWG);//(DP) or (DS)
    double wCoperWireCrossSectAreaPost(double &WireDiam);//
    double wCurrentDenst(FBTransformer &fbtvalue, double &WireAreaPost);//(JP) or (JS)
    double wNumTurnToLay(double &WireDiam);//Number of turns per layer(NL)
    double wNumLay(FBTransformer &fbtvalue, double &NumTurPerLay);//(LNp)
    //Current for secondary layers
    double wSecondCurrPeak(FBTransformer &fbtvalue, double &TurnRatio, double &CoeffPwr);//Peak current(IAMax)
    double wSecondCurrRMS(FBTransformer &fbtvalue, double &CoeffPwr, double &TurnRatio);//RMS current(ISRMS)
    /*Winding*/
private:
    double M;
    double FCu;
    double AWGp;
    double Np;
    double INS;
};
#endif // FBPTRANSFORMER_H
