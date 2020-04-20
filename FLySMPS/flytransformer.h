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
    double CurrPriAver(BCap *bcvalue, FBTransformer *fbtvalue);//()
    double CurrPriPeakToPeak(BCap *bcvalue, FBTransformer *fbtvalue, InputValue *ivalue);//()
    double CurrPriMax(FBTransformer *fbtvalue);//()
    double CurrPriValley(FBTransformer *fbtvalue);//()
    double CurrPriRMS(FBTransformer *fbtvalue);//()

    //Otput drain-source mosfet voltage values
    double VDSOn();//
    double VDSMax();//

    double NPimaryAL();
    double NPimaryBA();
    //Core geometry coefficient(Kg)
    double CoreGC();
    //Cross-sectional area to Window area core(WaAe)
    double WaAe();
    //Maximum current density of the wire(Jm)
    double CurrDens();
    //The air-gap length(lg)
    double LengthAG();
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
private:
    double KRF;
};

#endif // FLYTRANSFORMER_H
