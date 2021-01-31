#ifndef POWSUPPSOLVE_H
#define POWSUPPSOLVE_H

#include <QObject>
#include <QVector>
#include "LoggingCategories.h"
#include "structdata.h"
#include "diodebridge.h"
#include "bulkcap.h"
#include "fbptransformer.h"
#include "swmosfet.h"
#include "diodeout.h"
#include "capout.h"
#include "outfilter.h"
#include "controlout.h"
#include "qcustomplot.h"

class PowSuppSolve: public QObject
{
    Q_OBJECT
public:
    explicit PowSuppSolve(QObject *parent = nullptr);
    ~PowSuppSolve();

public slots:
    void startSolve();
    void stopSolve();

signals:
    void solveStarted();
    void solveFinished();
    void solveCanceled();

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
        int16_t sec_curr_peak;
        int16_t sec_out_voltage;
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

    };

private:
    bool m_isSolveRunning;
};

#endif // POWSUPPSOLVE_H
