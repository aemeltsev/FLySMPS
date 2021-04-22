#ifndef POWSUPPSOLVE_H
#define POWSUPPSOLVE_H

#include <QObject>
#include <QVector>
#include "LoggingCategories.h"
#include "diodebridge.h"
#include "bulkcap.h"
#include "fbptransformer.h"
#include "swmosfet.h"
#include "diodeout.h"
#include "capout.h"
#include "outfilter.h"
#include "controlout.h"

class PowSuppSolve: public QObject
{
    Q_OBJECT
public:
    explicit PowSuppSolve(QObject *parent = nullptr);
    ~PowSuppSolve();

public:
    void setGeneralSecondaryValues(
            int16_t p_volt_out_one,
            int16_t p_curr_out_one,
            int16_t p_volt_out_two,
            int16_t p_curr_out_two,
            int16_t p_volt_out_three,
            int16_t p_curr_out_three,
            int16_t p_volt_out_four,
            int16_t p_curr_out_four,
            int16_t p_volt_out_aux,
            int16_t p_curr_out_aux
            );
    void setGeneralInitValues(
            int16_t p_input_volt_ac_max,
            int16_t p_input_volt_ac_min,
            int16_t p_freq_line,
            int16_t p_freq_switch,
            int16_t p_temp_amb,
            double p_eff,
            double p_power_out_max
            );
    void setGeneralPreDesign(
            int16_t p_refl_volt_max,
            double p_eff_transf,
            int16_t p_voltage_spike,
            double p_volt_diode_drop_sec,
            double p_volt_diode_drop_bridge,
            double pleakage_induct
            );
    void setOutputCap(
            double p_sec_voltage_ripple,
            double p_sec_esr_perc,
            double p_sec_crfq_value
            );
    void setTransformerPreDesign(
            double p_mag_flux_dens,
            double p_win_util_factor,
            int16_t p_max_curr_dens,
            double p_al_induct_factor
            );
    void setTransformerCoreParam(
            
            );

public slots:
    void calcInputNetwork();
    void calcTransformetNetwork();
    void calcSwitchNetwork();
    void calcOtputNetwork();
    void calcPowerStageModel();
    void calcOptocouplerFeedback();

signals:
    void startCalcInputNetwork();
    void finishedInputNetwork();
    void startCalcTransformer();
    void finishedCalcTransformer();
    void startCalcSwitchNetwork();
    void finishedCalcSwitchNetwork();
    void startCalcPowerStageModel();
    void finishedCalcPowerStageModel();
    void startCalcOptocouplerFeedback();
    void finishedCalcOptocouplerFeedback();

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
        double eff_transf;
        int16_t voltage_spike;
        double volt_diode_drop_sec;
        double volt_diode_drop_bridge;
        double leakage_induct;
        //for cap out
        double sec_voltage_ripple;
        double sec_esr_perc;
        double sec_crfq_value;
    };

    struct PulseTransPreDesign
    {
        double mag_flux_dens;
        double win_util_factor;
        int16_t max_curr_dens;
        double al_induct_factor;
    };

    InputValue *m_indata;
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
        double number_primary;
        double actual_num_primary;

        double curr_primary_aver;//Primary average current during turn-on
        double curr_primary_peak_peak;//Primary peak-to-peak current
        double curr_primary_peak;//Primary peak current
        double curr_primary_valley;//Primary valley current
        double curr_primary_rms;//Primary RMS current

        double core_area_product;//Core area product Ap
        double core_win_core_sect;////Product of areas factor W_a*A_e
        //double area_wind_tot;
        double curr_dens;//
        double length_air_gap;//Air-gap length considered with fringing effect

        double actual_flux_dens_peak;//Calc peak flux density
        double actual_volt_reflected;//Recalc reflected voltage
        double actual_max_duty_cycle;//Recalc maximum duty cycle
        double fring_flux_fact;//
    };

    struct PulseTransSecondWired
    {
        QVector<double> out_one_wind;
        QVector<double> out_two_wind;
        QVector<double> out_three_wind;
        QVector<double> out_four_wind;
        QVector<double> out_aux_wind;
        QVector<double> primary_wind;
    };

    DBridge *m_db;
    BCap *m_bc;
    PMosfet *m_pm;
    ODiode *m_od;
    FullOutDiode *m_fod;
    OCap *m_oc;
    FullOutCap *m_foc;
    FullOutFilter *m_of;
    PowerStageSmallSignalModel *m_pssm;
    OptocouplerFedbackStage *m_ofs;
    PulseTransPrimaryElectr *m_ptpe;
    PulseTransSecondWired *m_ptsw;
    // out containers

    void calcBulkCapacitor(BulkCap *p_bcap);
    void calcDiodeBridge(DiodeBridge *p_bdiode);
    void calcTransformerPrimary(FBPTPrimary *p_pwtrprim);
    void calcTransformerCore(FBPTCore *p_pwtrcore);
    void calcTransformerSecondary(FBPTSecondary *p_pwrtsec);
    void calcTransformerWinding(FBPTWinding *p_pwtrwind);
    void calcPowerMosfet(SwMosfet *p_swmos);
    void calcDiodeOut(DiodeOut *p_do);
    void calcCapacitorOut(CapOut *p_co);
    void calcFilterOut(OutFilter *p_outfl);
    void calcPowerStage(PCSSM *p_pcssm);
    void calcOptocoupler(FCCD *p_fccd);
    bool m_isSolveRunning;
};

#endif // POWSUPPSOLVE_H
