#ifndef POWSUPPSOLVE_H
#define POWSUPPSOLVE_H

#include <QObject>
#include <QScopedPointer>
#include <QVector>
#include <QPair>
#include <QHash>
#include "LoggingCategories.h"
#include "diodebridge.h"
#include "bulkcap.h"
#include "fbptransformer.h"
#include "swmosfet.h"
#include "diodeout.h"
#include "capout.h"
#include "outfilter.h"
#include "controlout.h"

#define SET_SECONDARY_WIRED 4

class PowSuppSolve: public QObject
{
    Q_OBJECT
public:
    explicit PowSuppSolve(QObject *parent = nullptr);
    ~PowSuppSolve();

public slots:
    void calcInputNetwork();
    //Calculate transformer
    void calcElectricalPrimarySide();
    void calcArea();
    void calcElectroMagProperties();
    void calcTransformerWired();
    //Calculate transformer
    void calcSwitchNetwork();
    void calcOtputNetwork();
    void calcPowerStageModel();
    void calcOptocouplerFeedback();

signals:
    void startCalcInputNetwork();
    void finishedInputNetwork();
    void startCalcElectricalPrimarySide();
    void finishedCalcElectricalPrimarySide();
    void startCalcArea();
    void finishedCalcArea();
    void startCalcElectroMagProperties();
    void finishedCalcElectroMagProperties();
    void startCalcTransformerWired();
    void finishedCalcTransformerWired();
    void startCalcTransformer();
    void finishedCalcTransformer();
    void startCalcSwitchNetwork();
    void finishedCalcSwitchNetwork();
    void startCalcPowerStageModel();
    void finishedCalcPowerStageModel();
    void startCalcOptocouplerFeedback();
    void finishedCalcOptocouplerFeedback();
    void calcCanceled();

private:
    //input containers
    struct InputValue
    {
        int16_t input_volt_ac_max;
        int16_t input_volt_ac_min;
        int16_t freq_line;
        int16_t freq_switch;
        int16_t temp_amb;
        //Input secondary voltage, current value
        int16_t volt_out_one;
        int16_t curr_out_one;
        int16_t volt_out_two;
        int16_t curr_out_two;
        int16_t volt_out_three;
        int16_t curr_out_three;
        int16_t volt_out_four;
        int16_t curr_out_four;
        int16_t volt_out_aux;
        int16_t curr_out_aux;
        double eff;
        double power_out_max;
        //Pre-design
        int16_t refl_volt_max;
        int16_t voltage_spike;
        float ripple_fact;
        float eff_transf;
        float volt_diode_drop_sec;
        float volt_diode_drop_bridge;
        double leakage_induct;
        //for cap out
        double sec_voltage_ripple;
        float sec_esr_perc;
        double sec_crfq_value;
        float mrgn; /**< margin of the output power */
    };

    struct TransWired
    {
        /** [0]-Primary area coefficient,
         *  [1]-1st area coefficient,
         *  [2]-2nd area coefficient,
         *  [3]-3th area coefficient,
         *  [4]-4th area coefficient,
         *  [5]-Aux area coefficient */
        QVector<float> m_af;
        /** [0]-Primary insulation coefficient,
         *  [1]-1st wired insulation coefficient,
         *  [2]-2nd wired insulation coefficient,
         *  [3]-3th wired insulation coefficient,
         *  [4]-4th wired insulation coefficient,
         *  [5]-Aux insulation coefficient */
        QVector<float> m_ins;
        int16_t m_mcd; /**< Safety standart margin */
        float m_fcu; /**< Copper space factor */
    };
    //input containers

    // out containers
    struct DBridge
    {
        double diode_peak_curr;
        double diode_rms_curr;
        double diode_avg_curr;
        double diode_rms_curr_tot;
        double load_avg_curr;
        double diode_curr_slope;
        double diode_cond_time;
        double in_min_rms_voltage;
        double in_max_rms_voltage;
    };

    struct BCap
    {
        double delta_t;
        double charg_time;
        double bcapacitor_value;
        double load_curr_max;
        double load_curr_min;
        double bcapacitor_peak_curr;
        double bcapacitor_rms_curr;
        double input_min_voltage;
        double input_dc_min_voltage;
    };

    struct PMosfet
    {
        double mosfet_voltage_nom;
        double mosfet_voltage_max;
        double mosfet_rise_time;

        double mosfet_conduct_loss;
        double mosfet_drive_loss;
        double mosfet_switch_loss;
        double mosfet_capacit_loss;

        double snubber_voltage_max;
        double snubber_pwr_diss;
        double snubber_res_value;
        double snubber_cap_value;

        double curr_sense_res;
        double curr_sense_res_loss;
    };

    struct ODiode
    {
        double diode_rev_voltage;
        double diode_pow_diss;
    };

    struct FullOutDiode
    {
        double sec_out_pwr;
        int16_t sec_out_voltage;
        double turn_ratio;
        ODiode doout;
    };

    struct OCap
    {
        double cap_value_out;
        double cap_esr_out;
        double cap_curr_rms;
        double cap_zfc_out;
        double cap_rippl_volr_out;
        double cap_out_loss;
    };

    struct FullOutCap
    {
        double sec_voltage_ripple;
        double sec_esr_perc;
        double sec_crfq_value;
        int16_t sec_curr_peak;// from input value
        int16_t sec_out_voltage;// from input value
        OCap capout;
    };

    struct FullOutFilter
     {
         int16_t frequency;
         int16_t load_resistance;
         double angular_cut_freq;
         double capacitor;
         double inductor;
         double q_factor;
         double damping;
         double cut_freq;
         double out_ripp_voltage;
         QVector<double> of_freq_array;
         QVector<double> of_magnitude_array;
         QVector<double> of_phase_array;
     };

    struct PowerStageSmallSignalModel
    {
        double ps_zero_one;
        double ps_pole_one;
        double ps_dcm_zero_two;
        double ps_dcm_pole_two;
        double ps_ccm_zero_two;
        double ps_ccm_pole_two;
        double ps_gain_cmc_mod;
        QVector<double> ps_freq_array;
        QVector<double> ps_magnitude_array;
        QVector<double> ps_phase_array;
    };

    struct OptocouplerFedbackStage
    {
        int16_t of_opto_led_res;
        int16_t of_opto_bias_res;
        int16_t of_up_divide_res;
        double of_quality;
        double of_ext_ramp_slope;
        double of_ind_on_slope;
        double of_freq_cross_sect;
        double of_zero;
        double of_pole;
        double of_cap_opto;
        double of_res_err_amp;
        double of_cap_err_amp;
        QVector<double> of_freq_array;
        QVector<double> of_magnitude_array;
        QVector<double> of_phase_array;
    };

    struct PulseTransPrimaryElectr
    {
        double max_duty_cycle;//Max duty cycle
        double inp_power;//Input power
        double primary_induct;//Primary inductance
        int32_t number_primary;
        int32_t actual_num_primary;

        double curr_primary_aver;//Primary average current during turn-on
        double curr_primary_peak_peak;//Primary peak-to-peak current
        double curr_primary_peak;//Primary peak current
        double curr_primary_valley;//Primary valley current
        double curr_primary_rms;//Primary RMS current

        double core_area_product;//Core area product(A_p)
        double core_geom_coeff;////The core geometry coefficient(K_g)
        //double area_wind_tot;
        double curr_dens;//
        double length_air_gap;//Air-gap length considered with fringing effect

        double actual_flux_dens_peak;//Calc peak flux density
        double actual_volt_reflected;//Recalc reflected voltage
        double actual_max_duty_cycle;//Recalc maximum duty cycle
        double fring_flux_fact;//
    };

    /**
     * @brief The PulseTransWires struct
     *        Secondary hash:                                        Primary hash:
     *        JSP - Peak current for secondary layer                 AP - Wire copper area for primary winding
     *        JSRMS - RMS current for secondary layer                AWGP - Wire size in AWG unit
     *        NSEC - Number turn for secondary layer                 DP - Primary wire diameter from cooper area
     *        ANS - Wire copper area for secondsry winding           ECA - Effective copper area
     *        AWGNS - Wire size in AWG unit                          JP - Current density
     *        DS - Secondary wire diameter from cooper area          OD - Wire outer diameter including insulation
     *        ECA - Effective copper area                            NTL - Max number of turns per layer
     *        JS - Current density                                   LN - Min number of layers
     *        OD - Wire outer diameter including insulation
     *        NTL - Max number of turns per layer
     *        LN - Min number of layers
     */
    struct PulseTransWires
    {
        QHash<QString, float> out_one_wind;
        QHash<QString, float> out_two_wind;
        QHash<QString, float> out_three_wind;
        QHash<QString, float> out_four_wind;
        QHash<QString, float> out_aux_wind;
        QHash<QString, float> primary_wind;
    };
    // out containers

    bool m_isSolveRunning = false;
    QScopedPointer<FBPTCore> m_core;
    QVector<QScopedPointer<FBPTSecondary>> m_sec;
    QVector<QScopedPointer<FBPTWinding>> m_wind;

public:
    InputValue m_indata;
    CoreArea m_ca;
    CoreSelection m_cs;
    MechDimension m_md;
    FBPT_NUM_SETTING m_fns;
    FBPT_SHAPE_AIR_GAP m_fsag;
    TransWired m_psw;

    QScopedPointer<DBridge> m_db;
    QScopedPointer<BCap> m_bc;
    QScopedPointer<PMosfet> m_pm;
    QScopedPointer<PulseTransPrimaryElectr> m_ptpe;
    QScopedPointer<PulseTransWires> m_ptsw;
    //ODiode m_od;
    QScopedPointer<FullOutDiode> m_fod;
    //OCap m_oc;
    QScopedPointer<FullOutCap> m_foc;
    QScopedPointer<FullOutFilter> m_of;
    QScopedPointer<PowerStageSmallSignalModel> m_pssm;
    QScopedPointer<OptocouplerFedbackStage> m_ofs;
};
#endif // POWSUPPSOLVE_H
