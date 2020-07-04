#ifndef STRUCTDATA_H
#define STRUCTDATA_H
#include <cstdint>

#define S_PI       3.1415926535897932385
#define S_RO_OM    1.72E-8 //Ohm/m
#define S_MU_Z     4.*S_PI*1E-7 //H/m
#define S_K_1      85*1E-4

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
};
//
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
//
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
//
struct FBPT
{
    double max_duty_cycle;//Max duty cycle
    double inp_power;//Input power
    double primary_induct;//Primary inductance

    double curr_primary_aver;//Primary average current during turn-on
    double curr_primary_peak_peak;//Primary peak-to-peak current
    double curr_primary_peak;//Primary peak current
    double curr_primary_valley;//Primary valley current
    double curr_primary_rms;//Primary RMS current

    double area_wind_tot;//
    double length_air_gap;//
};

struct FBTransformer
{


    double core_area_product;//Core area product Ap
    double core_win_core_sect;////Product of areas factor W_a*A_e
    double delta_flux_max;//
    double curr_dens;//

    double number_primary_al;//Calculate primary number of turns use - AL value (inductance factor)
    double number_primary_ba;//Calculate minimum primary number of turns use B_max(max. flux density)
    double number_primary_wa;//

    double length_air_gap;//Air-gap length considered with fringing effect
    double fring_area;//
    double fring_flux_fact;//
    double actual_num_primary;
    double actual_b_peak;//Calc peak flux density
    double actual_volt_reflected;//Recalc reflected voltage
    double actual_max_duty_cycle;//Recalc maximum duty cycle
};
//
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

#endif // STRUCTDATA_H
