#ifndef STRUCTDATA_H
#define STRUCTDATA_H
#include <cstdint>
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
    //Pre-design
    int16_t refl_volt_max;
    double eff_transf;
    int16_t voltage_spike;
    double volt_diode_drop_sec;
    double volt_diode_drop_bridge;
};

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

#endif // STRUCTDATA_H
