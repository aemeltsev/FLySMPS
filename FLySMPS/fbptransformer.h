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

    /*Core Geometry Factor and Core Selection*/
    double CoreAreaProd(const FBPT &fbptval);//Core geometry coefficient(Ap)
    double CoreWinToCoreSect(const FBPT &fbptval);//Cross-sectional area to Window area core(WaAe)
    double DeltaFluxMax(const FBPT &fbptval);
    void setCoreSelection(double ap, double mu_rc, double ac, double wa, double vc, double lt, double lc, double hw, double al);
    double AreaWindTotal(const FBPT &fbptval, InputValue &ivalue);//Cross-sectional area of the winding bare wire
    double CurrentDens(const FBPT &fbptval);
    /*Core Geometry Factor and Core Selection*/
private:
    double curr_dens;//Jm - the maximum current density
    double core_win_util_fact;//Ku - window utilization factor
    double flux_dens_max;//Bm - saturation magnetic field density
    double EnergyStoredChoke(const FBPT &fbptval);//
};

class FBPTWinding
{
public:
    FBPTWinding();

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

};
#endif // FBPTRANSFORMER_H
