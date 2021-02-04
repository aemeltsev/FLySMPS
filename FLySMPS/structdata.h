#ifndef STRUCTDATA_H
#define STRUCTDATA_H
#include <cstdint>

namespace indata{

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

struct PulseTransMechanical
{

};
}


#endif // STRUCTDATA_H
