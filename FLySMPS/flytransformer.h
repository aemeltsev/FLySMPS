/**
  * License text...
*/
#ifndef FLYTRANSFORMER_H
#define FLYTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class FlyTransformer
{
public:
    FlyTransformer();


    double numPimaryAL(FBTransformer &fbtvalue);//Primary turns using inductance factor
    double numPimaryBA(FBTransformer &fbtvalue);//Primary turns using core area
    double numPrimaryWireArea(FBTransformer &fbtvalue);//

    /*Air-Gap Length Considered with Fringing Effect*/
    double agLength(FBTransformer &fbtvalue, double &varNumPrim);//The air-gap length(lg)
    void setMechanDimension(double f, double c, double e, double d);
    double agFringArea(FBTransformer &fbtvalue);
    double agFringFluxFact(FBTransformer &fbtvalue);//Correction factor F. - the edge coefficient(FFC)
    /*Air-Gap Length Considered with Fringing Effect*/
    
    /*Recalc Np, Bm, RefVoltage, DutyCycle*/
    double actNumPrimary(FBTransformer &fbtvalue);
    double actFluxDensPeak(FBTransformer &fbtvalue);//Actual flux density(BmAct)
    double actVoltageRefl(InputValue &ivalue, FBTransformer &fbtvalue, double &varNumSec);//Post-calculated reflected voltage(VRPost)
    double actMaxDutyCycle(FBTransformer &fbtvalue, BCap &bcvalue);//Post-calculated maximum duty cycle(DMaxPost)
    /*Recalc Np, Bm, RefVoltage, DutyCycle*/
    
    /**/
    double numOutPower(double &OutCurr, double &OutVolt);//Power of output n
    double numCoeffPower(InputValue &ivalue, double &OutPower);//Output power coefficient(Kl)
    double numSecondary(FBTransformer &fbtvalue, InputValue &ivalue, double &OutVolt);
    double numTurnsRatio(FBTransformer &fbtvalue, double &NumTurns);//Primary to secondary turns ratio
    /**/


private:

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

    /* Mechanical dimensions */
    /* Rectangular Air Gap */
    double F;
    double C;
    double E;
    double D;
    /* Round Air Gap */
    double Diam;
    /* Mechanical dimensions */

    double M;
    double FCu;
    double AWGp;
    double Np;
    double INS;
};

#endif // FLYTRANSFORMER_H
