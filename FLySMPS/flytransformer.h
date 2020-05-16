#ifndef FLYTRANSFORMER_H
#define FLYTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class FlyTransformer
{
public:
    FlyTransformer();

    double DutyCycleDCM(InputValue *ivalue, BCap *bcvalue);//Maximum duty cycle
    double InputPower(InputValue *ivalue);//Maximum input power
    double PriInduct(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue);//Output primary inductance

    //All current primary side
    double CurrPriAver(BCap *bcvalue, FBTransformer *fbtvalue);//
    double CurrPriPeakToPeak(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue);//
    double CurrPriMax(FBTransformer *fbtvalue);//
    double CurrPriValley(FBTransformer *fbtvalue);//
    double CurrPriRMS(FBTransformer *fbtvalue);//

    void setMagneteValues(double currdens, double utilfact, double fluxdens, double krf);

    double CoreAreaProd(FBTransformer *fbtvalue);//Core geometry coefficient(Ap)

    double CoreWinToCoreSect(FBTransformer *fbtvalue, InputValue *ivalue);//Cross-sectional area to Window area core(WaAe)

    void setCoreSelection(double ap, double mu_rc, double ac, double wa, double vc, double lt, double lc, double hw, double al);

    double NPimaryAL(FBTransformer *fbtvalue);//Primary turns using inductance factor
    double NPimaryBA(FBTransformer *fbtvalue);//Primary turns using core area

    double WireAreaWind(FBTransformer *fbtvalue, bool cff);//Cross-sectional area of the winding bare wire
    double CurrDens(FBTransformer *fbtvalue);//Maximum current density of the wire(Jm)
    double NPrimaryWireArea(FBTransformer *fbtvalue);//

    double LengthAirGap();//The air-gap length(lg)
    //Actual flux density(BMaxAct)
    double BMaxAct();
    //Post-calculated reflected voltage(VRPost)
    double VRPost();
    //Post-calculated maximum duty cycle(DMaxPost)
    double MaxDutyCyclePost();
    //Correction factor F. - the edge coefficient(FFC)
    double FringFluxFact();
    //Power of output n
    double NPower();
    //Output power coefficient(Kl)
    double NCfPower();
    //Effective bobbin width(BWe)
    double WidthBE();
    //Effective winding cross-section(Wae)
    double EffectAe();


    //Winding
    double CopAeP();//(AP) or (ANS)
    double WireAWG();//(AWGP) or (AWGNS)
    double CopDiam();//(DP) or (DS)
    double CopAePPost();//
    double CurrDes();//(JP) or (JS)
    double NumTurToLay();//Number of turns per layer(NL)
    double NumLay();//(LNp)
    //Secondary number of turns
    double NSecondary();
    double TRatio();//Primary to secondary turns ratio
    double SPeakCurr();//Peak current(IAMax)
    double ISecRMS();//RMS current(ISRMS)

    //Otput drain-source mosfet voltage values
    double VDSOn();//

    double VDSMax();//

private:

    double KRF;

    double curr_dens;//Jm - the maximum current density
    double core_win_util_fact;//Ku - window utilization factor
    double flux_dens_max;//Bm - saturation magnetic field density
    double ElectrCondCoeff();
    double EnergyStoredChoke(FBTransformer *fbtvalue);//

    /* Core selection */
    double core_area_product;//Ap
    double core_permeal;//mu_rc(mu_r - relative permeability TDK)
    double core_cross_sect_area;//Ac(Ae - effective magnetic cross section TDK)
    double core_wind_area;//Wa(An - winding cross section TDK)
    double core_vol;//Vc(Ve - effective magnetic volume TDK)
    double mean_leng_per_turn;//l_t(l_n - average length of turn TDK)
    double mean_mag_path_leng;//l_c(l_e - effective magnetic path length TDK)
    double core_win_height;//height of the window
    double ind_fact;//Al(inductance factor TDK)
    /* Core selection */





};

#endif // FLYTRANSFORMER_H
