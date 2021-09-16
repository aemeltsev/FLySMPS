/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef POWSUPPSOLVE_H
#define POWSUPPSOLVE_H

#include <QObject>
#include <QScopedPointer>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QHash>
#include <QtMath>
#include <QDebug>
#include "diodebridge.h"
#include "bulkcap.h"
#include "fbptransformer.h"
#include "swmosfet.h"
#include "diodeout.h"
#include "capout.h"
#include "outfilter.h"
#include "controlout.h"

#define SET_SECONDARY_WIRED 4
#define SET_FREQ_SIZE 1*1E7 //10MHz

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
    void calcOutputFilter();
    void calcPowerStageModel();
    void calcOptocouplerFeedback();

signals:
    void finishedCalcInputNetwork();
    void finishedCalcElectricalPrimarySide();
    void finishedCalcArea();
    void finishedCalcElectroMagProperties();
    void finishedCalcTransformerWired();
    void finishedCalcSwitchNetwork();
    void finishedCalcOtputNetwork();
    void newOFDataHash(QHash<QString, double>);
    void newOFDataPlot(QVector<double>, QVector<double>);
    void finishedCalcOutputFilter();
    void newPSMDataHash(QHash<QString, double>);
    void newPSMDataPlot(QVector<double>, QVector<double>);
    void finishedCalcPowerStageModel();
    void newOCFDataHash(QHash<QString, double>);
    void newOCFDataPlot(QVector<double>, QVector<double>);
    void finishedCalcOptocouplerFeedback();
    void calcFinished();

private:
    //input containers
    struct InputValue
    {
        int16_t input_volt_ac_max;
        int16_t input_volt_ac_min;
        int16_t freq_line;
        uint32_t freq_switch;
        int16_t temp_amb;
        //Input secondary voltage, current value
        int16_t volt_out_one;
        float curr_out_one;
        int16_t volt_out_two;
        float curr_out_two;
        int16_t volt_out_three;
        float curr_out_three;
        int16_t volt_out_four;
        float curr_out_four;
        int16_t volt_out_aux;
        float curr_out_aux;
        double eff;
        double power_out_max;
        //Pre-design
        int16_t refl_volt_max;
        uint16_t voltage_spike;
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
        //for out filter
        int32_t fl_freq;
        int32_t fl_lres;
        
    };

    struct TransWired
    {
        /** [0]-Primary area coefficient,
         *  [1]-1st area coefficient ... [4]-4th area coefficient,
         *  [5]-Aux area coefficient */
        QVector<float> m_af;
        /** [0]-Primary insulation coefficient,
         *  [1]-1st wired insulation coefficient ... [4]-4th wired insulation coefficient,
         *  [5]-Aux insulation coefficient */
        QVector<float> m_ins;
        QVector<int16_t> m_npw;
        float m_mcd; /**< Safety standart margin */
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
        int16_t mosfet_voltage_nom;
        int16_t mosfet_voltage_max;
        float mosfet_ds_curr;
        double mosfet_on_time;
        double mosfet_off_time;
        double mosfet_fall_time;
        double mosfet_rise_time;

        float mosfet_conduct_loss;
        float mosfet_drive_loss;
        float mosfet_switch_loss;
        float mosfet_capacit_loss;
        float mosfet_total_loss;

        int32_t snubber_voltage_max;
        float snubber_pwr_diss;
        int32_t snubber_res_value;
        double snubber_cap_value;

        float curr_sense_res;
        float curr_sense_res_loss;
    };

    /**
     * @brief The FullOutDiode struct
     *        SOP - Secondary output power
     *        SOV - Secondary output voltage
     *        TR - Turns ratio
     *        DRV - Output diode reverse voltage
     *        DPD - Output diode power dissipation
     */
    struct FullOutDiode
    {
        QHash<QString, float> out_diode_first;
        QHash<QString, float> out_diode_sec;
        QHash<QString, float> out_diode_thrid;
        QHash<QString, float> out_diode_four;
        QHash<QString, float> out_diode_aux;
    };

    /**
     * @brief The FullOutCap struct
     *        CVO - Output capacitor value
     *        CESRO - Calculated output capacitor ESR
     *        CCRMS - Output capacitor current RMS
     *        CZFCO - Zero frequency capacitor output
     *        CRVO - Output capacitor ripple voltage
     *        COL - Output capacitor loss
     */
    struct FullOutCap
    {        
        QHash<QString, float> out_cap_first;
        QHash<QString, float> out_cap_sec;
        QHash<QString, float> out_cap_thrid;
        QHash<QString, float> out_cap_four;
        QHash<QString, float> out_cap_aux;
    };

    struct FullOutFilter
     {
        int32_t m_frequency;
        int32_t m_load_resistance;

        QHash<QString, double> m_of_var_filter;
        QVector<double> m_of_freq_array;
        QVector<double> m_of_magnitude_array;
        QVector<double> m_of_phase_array;

        explicit FullOutFilter(int32_t frq, int32_t r_load) noexcept
            :m_frequency(frq)
            ,m_load_resistance(r_load)
        {}

        void ofAddData(double angular_cut_freq,
                       double capacitor,
                       double inductor,
                       double q_factor,
                       double damping,
                       double cut_freq,
                       double out_ripp_voltage)
        {
            m_of_var_filter.insert("ACF", angular_cut_freq);
            m_of_var_filter.insert("CAP", capacitor);
            m_of_var_filter.insert("IND", inductor);
            m_of_var_filter.insert("QFCT", q_factor);
            m_of_var_filter.insert("DAMP", damping);
            m_of_var_filter.insert("CFRQ", cut_freq);
            m_of_var_filter.insert("ORV", out_ripp_voltage);
        }
     };

    struct PowerStageSmallSignalModel
    {
        QHash<QString, double> m_ps_ssm;
        QVector<double> m_ps_freq_array;
        QVector<double> m_ps_magnitude_array;
        QVector<double> m_ps_phase_array;

        PowerStageSmallSignalModel() noexcept
        {}

        void ssmAddData(double ps_zero_one,
                        double ps_pole_one,
                        double ps_gain_cmc_mod=0.0,
                        double ps_dcm_zero_two=0.0,
                        double ps_dcm_pole_two=0.0,
                        double ps_ccm_zero_two=0.0,
                        double ps_ccm_pole_two=0.0)
        {
            m_ps_ssm.insert("ZONE", ps_zero_one);
            m_ps_ssm.insert("PONE", ps_pole_one);
            m_ps_ssm.insert("DCMZT", ps_dcm_zero_two);
            m_ps_ssm.insert("DCMPT", ps_dcm_pole_two);
            m_ps_ssm.insert("CCMZT", ps_ccm_zero_two);
            m_ps_ssm.insert("CCMPT", ps_ccm_pole_two);
            m_ps_ssm.insert("GCMC", ps_gain_cmc_mod);
        }
    };

    struct OptocouplerFedbackStage
    {
        QHash<QString, double> m_ofs_data;
        QVector<double> m_ofs_freq_array;
        QVector<double> m_ofs_magnitude_array;
        QVector<double> m_ofs_phase_array;

        OptocouplerFedbackStage() noexcept
        {}

        void ofsAddData(double ofs_opto_led_res,
                        double ofs_opto_bias_res,
                        double ofs_up_divide_res,
                        double ofs_quality,
                        double ofs_ext_ramp_slope,
                        double ofs_ind_on_slope,
                        double ofs_freq_cross_sect,
                        double ofs_zero,
                        double ofs_pole,
                        double ofs_cap_opto,
                        double ofs_res_err_amp,
                        double ofs_cap_err_amp)
        {
            m_ofs_data.insert("RESOPTLED", ofs_opto_led_res);
            m_ofs_data.insert("RESOPTBIAS", ofs_opto_bias_res);
            m_ofs_data.insert("RESUPDIV", ofs_up_divide_res);
            m_ofs_data.insert("QUAL", ofs_quality);
            m_ofs_data.insert("RS", ofs_ext_ramp_slope);
            m_ofs_data.insert("IOS", ofs_ind_on_slope);
            m_ofs_data.insert("FCS", ofs_freq_cross_sect);
            m_ofs_data.insert("OFSZ", ofs_zero);
            m_ofs_data.insert("OFSP", ofs_pole);
            m_ofs_data.insert("CAPOPTO", ofs_cap_opto);
            m_ofs_data.insert("RESERR", ofs_res_err_amp);
            m_ofs_data.insert("CAPERR", ofs_cap_err_amp);
        }
    };

    struct PulseTransPrimaryElectr
    {
        double max_duty_cycle;//Max duty cycle
        double inp_power;//Input power
        double primary_induct;//Primary inductance
        uint32_t number_primary;
        uint32_t actual_num_primary;

        double curr_primary_aver;//Primary average current during turn-on
        double curr_primary_peak_peak;//Primary peak-to-peak current
        double curr_primary_peak;//Primary peak current
        double curr_primary_valley;//Primary valley current
        double curr_primary_rms;//Primary RMS current

        double core_area_product;//Core area product(A_p)
        double core_geom_coeff;//The core geometry coefficient(K_g)
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

    bool m_abort;
    bool m_working;
    QMutex m_mutex;

    QScopedPointer<FBPTCore> m_core;
    QVector<QSharedPointer<FBPTSecondary>> m_sec;
    QVector<QSharedPointer<FBPTWinding>> m_wind;
    QScopedPointer<PCSSM> m_pcssm;
    QScopedPointer<FCCD> m_fccd;

public:
    InputValue m_indata;
    CoreArea m_ca;
    CoreSelection m_cs;
    MechDimension m_md;
    FBPT_NUM_SETTING m_fns;
    FBPT_SHAPE_AIR_GAP m_fsag;
    TransWired m_psw;
    MosfetProp m_mospr;
    ClampCSProp m_ccsp;
    QVector<CapOutProp> m_cop;
    SSMPreDesign m_ssm;
    PS_MODE m_psm;
    FCPreDesign m_fc;
    RampSlopePreDesign m_rs;
    LCSecondStage m_lc;

    QScopedPointer<BCap> m_bc;
    QScopedPointer<DBridge> m_db;
    QScopedPointer<PMosfet> m_pm;
    QScopedPointer<PulseTransPrimaryElectr> m_ptpe;
    QScopedPointer<PulseTransWires> m_ptsw;
    QScopedPointer<FullOutDiode> m_fod;
    QScopedPointer<FullOutCap> m_foc;
    QScopedPointer<FullOutFilter> m_of;
    QScopedPointer<PowerStageSmallSignalModel> m_pssm;
    QScopedPointer<OptocouplerFedbackStage> m_ofs;
};
#endif // POWSUPPSOLVE_H
