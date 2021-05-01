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
    double core_win_height;//height of the window
    double ind_fact;//Al(inductance factor TDK)
    /**< Core parameters */
    double core_cross_sect_area;//Ac(Ae - effective magnetic cross section TDK)
    double core_wind_area;//Wa(An - winding cross section TDK)
    double core_vol;//Vc(Ve - effective magnetic volume TDK)
    double mean_leng_per_turn;//l_t(l_n - average length of turn TDK)
    double mean_mag_path_leng;//l_c(l_e - effective magnetic path length TDK)
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

    inline double EnergyStoredChoke() const;//
    inline double AreaWindTotal(const CoreSelection &cs) const;//Cross-sectional area of the winding bare wire

public:
    inline double CoreAreaProd() const;//Core geometry coefficient(Ap)
    inline double CoreWinToCoreSect() const;//Cross-sectional area to Window area core(WaAe)
    //Correction factor F. - the edge coefficient(FFC)
    double agFringFluxFact(const CoreSelection &cs, double varNumPrim, /*double ewff,*/
                           FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm,
                           double k=2.0) const;
    inline double CurrentDens(const CoreSelection &cs) const;
    /*Core Geometry Factor and Core Selection*/

    double numPrimary(const CoreSelection &cs, const FBPT_NUM_SETTING &fns);
    inline double agLength(const CoreSelection &cs, double varNumPrim) const;//The air-gap length(lg)
    /*Air-Gap Length Considered with Fringing Effect*/

    /*Recalc Np, Bm */
    inline int16_t actNumPrimary(const CoreSelection &cs,
                          FBPT_SHAPE_AIR_GAP &fsag, MechDimension &mchdm, /*double ewff,*/
                          double varNumPrim, double varIndPrim, double currPeakPrim,
                          double k=2.0) const;
    inline double actMagneticFluxPeak(const CoreSelection &cs, int16_t actNumPrim,
                                   double maxCurPrim, float agLength) const;
    inline float actDutyCycle(QVector<QPair<float, float>> outVtcr, int16_t in_volt_min,
                              int16_t fsw, double prim_ind) const;
    inline int16_t actReflVoltage(float actDuty, float maxOutPwr,
                                  double primInduct, int16_t fsw) const;
    /*Recalc Np, Bm */
};

//inline double postVoltageRefl(int16_t actual_num_primary, float voltout, float voltdrop, float numsec);//Post-calculated reflected voltage(VRPost)
//inline double postMaxDutyCycle(int16_t actual_volt_reflected, int16_t input_min_voltage);//Post-calculated maximum duty cycle(DMaxPost)

class FBPTSecondary
{
public:
    /**
     * @brief FBPTSecondary
     * @param currout
     * @param voltout
     * @param voltreflect
     * @param powout
     * @param aprnm
     * @param ddrp
     */
    FBPTSecondary(float currout, float voltout,
                  float voltreflect, float powout,
                  int16_t aprnm, float adc, float ddrp):
        curr(currout), volt(voltout),
        refl_volt(voltreflect), pow_out_max(powout),
        actual_num_primary(aprnm), actual_duty_cycle(adc), diode_drop_sec(ddrp)
    {}

    inline double outPWR() const {return static_cast<double>(curr*volt);}
    inline double outCoeffPWR() const {return outPWR()/ static_cast<double>(pow_out_max);}
    inline double outNumSecond() const;
    inline double outNumTurnRatio() const;

    void setCurrentParam(double pkcpr, double rmscpr){curr_primary_peak = pkcpr; curr_primary_rms = rmscpr;}
    //Current for secondary layers
    inline double outCurrPeakSecond();//Peak current(IAMax)
    inline double outCurrRMSSecond();//RMS current(ISRMS)
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
    FBPTWinding(int16_t aprnm, float frsw,
                float rmscp, int8_t m=4,
                double fcu=0.4, double ins=0.01):
        actual_num_primary(aprnm), freq_switch(frsw),
        curr_primary_rms(rmscp),
        M(m), FCu(fcu), INS(ins)
    {}
    /*Winding*/
    inline double wEffBobbWidth(const MechDimension &mchdm) const;//Effective bobbin width(BWe)
    inline double wEffWindCrossSect(const CoreSelection &cs, const MechDimension &mchdm) const;//Effective winding cross-section(ANe)
    inline double wCoperWireCrossSectArea(const CoreSelection &cs, const MechDimension &mchdm, double windfact) const;//(AP) or (ANS)
    inline double wMaxWireSizeAWG(double wirecrosssect) const;//(AWGP) or (AWGNS)
    inline double wSkinDepth() const;
    void setWireDiam(double awgp, uint16_t np);
    inline double wCoperWireDiam() const;//(DP) or (DS)
    inline double wCoperWireCrossSectAreaPost() const;//
    inline double wCurrentDenst() const;//(JP) or (JS)
    inline double wNumTurnToLay(const MechDimension &mchdm) const;//Number of turns per layer(NL)
    inline double wNumLay(const MechDimension &mchdm) const;//(LNp)
    /*Winding*/
private:
    int16_t actual_num_primary;
    float freq_switch;
    float curr_primary_rms;
    int8_t M;
    double FCu;
    double AWGp;
    double Np;
    double INS;
};
#endif // FBPTRANSFORMER_H
