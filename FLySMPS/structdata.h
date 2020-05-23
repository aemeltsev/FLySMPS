#ifndef STRUCTDATA_H
#define STRUCTDATA_H
#include <cstdint>

#define PI   3.1415926535897932385
#define MU_Z 4.*PI*1e-7
#define K_1  85*1E-4

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
struct FBTransformer
{
    double max_duty_cycle;//Max duty cycle
    double inp_power;//Input power
    double primary_induct;//Primary inductance

    double curr_primary_aver;//Primary average current during turn-on
    double curr_primary_peak_peak;//Primary peak-to-peak current
    double curr_primary_peak;//Primary peak current
    double curr_primary_valley;//Primary valley current
    double curr_primary_rms;//Primary RMS current

    double energy_stored_choke;
    double electr_cond_coeff;
    double core_area_product;//Core area product Ap

    double core_win_core_sect;////Product of areas factor W_a*A_e

    double number_primary_al;//Calculate primary number of turns use - AL value (inductance factor)
    double number_primary_bmax;//Calculate minimum primary number of turns use B_max(max. flux density)

    double wire_area_wind;//
    double number_primary_aw;//
    double length_air_gap;//Air-gap length considered with fringing effect

    double fring_flux_coeff;//Fringing effect factor depends from the length of the gap and cross-sectional area
    double fring_area;//
    double fring_flux_fact;//
    
    double actual_num_primary;
    double actual_b_peak;//Calc peak flux density
    double actual_volt_reflected;//Recalc reflected voltage
    double actual_max_duty_cycle;//Recalc maximum duty cycle

    double n_out_power;
    double n_out_coeff_power;

    //Winding
    double copper_ae;//(AP) or (ANS)
    double max_wire_awg;//(AWGP) or (AWGNS)
    double cop_wire_diam;//(DP) or (DS)
    double copper_ae_post;//
    double wire_curr_dens;//(JP) or (JS)
    double num_tur_per_lay;//Number of turns per layer(NL)
    double num_lay;//(LNp)

    //Secondary number of turns
    double turns_ratio;//Primary to secondary turns ratio
    double curr_peak_second;//Peak current(IAMax)
    double curr_rms_second;//RMS current(ISRMS)
    double numb_second;

};
//
struct PMosfet
{
    double volt_drain_sour_on;//Mosfet max. average drop on R_DS(on) in ON-stage
    double volt_drain_sour_max;//Maximum drain voltage in OFF-stage
};

#endif // STRUCTDATA_H
