#ifndef POWSUPPSOLVE_H
#define POWSUPPSOLVE_H

#include <QObject>
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

    struct OutDiode
    {
        double sec_pow_out;
        double sec_voltage_out;
        double turn_ratio;

        double diode_rev_voltage;
        double diode_pow_diss;
    };

    struct OutCap
    {
        double sec_voltage_ripple;
        double sec_esr_perc;
        double sec_curr_out;
        double sec_voltage_out;
        double sec_crfq_value;

        double cap_value_out;
        double cap_esr_out;
        double cap_curr_rms;
        double cap_zfc_out;
        double cap_rippl_volr_out;
        double cap_out_loss;
    };

private:
    bool m_isSolveRunning;
};

#endif // POWSUPPSOLVE_H
