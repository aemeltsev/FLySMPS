#include "FLySMPS.h"


FLySMPS::FLySMPS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FLySMPS),
    m_psolve(new PowSuppSolve)
{
    ui->setupUi(this);

    m_psworker = new QThread();
    initInputValues();

    m_psolve->moveToThread(m_psworker);
    connect(ui->InpCalcPushButton, &QPushButton::clicked, m_psolve.data(), &PowSuppSolve::calcInputNetwork);
    connect(m_psolve.data(), &PowSuppSolve::finishedInputNetwork, this, &FLySMPS::setInputNetwork);

}

FLySMPS::~FLySMPS()
{
    if(m_psworker->isRunning()){
        m_psworker->quit();
    }
    m_psworker->deleteLater();
}

void FLySMPS::initInputValues()
{
    m_psolve->m_indata.input_volt_ac_max = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VACmax->text())));
    m_psolve->m_indata.input_volt_ac_min = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VACmin->text())));
    m_psolve->m_indata.freq_line = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->FLine->text())));
    m_psolve->m_indata.freq_switch = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->FSw->text())));
    m_psolve->m_indata.temp_amb = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->Tamb->text())));
    m_psolve->m_indata.volt_out_one = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VOut1->text())));
    m_psolve->m_indata.curr_out_one = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->IOut1->text())));
    m_psolve->m_indata.volt_out_two = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VOut2->text())));
    m_psolve->m_indata.curr_out_two = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->IOut2->text())));
    m_psolve->m_indata.volt_out_three = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VOut3->text())));
    m_psolve->m_indata.curr_out_three = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->IOut3->text())));
    m_psolve->m_indata.volt_out_four = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VOut4->text())));
    m_psolve->m_indata.curr_out_four = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->IOut4->text())));
    m_psolve->m_indata.volt_out_aux = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VAux->text())));
    m_psolve->m_indata.curr_out_aux = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->IAux->text())));
    m_psolve->m_indata.eff = convertToValues(static_cast<QString>(ui->Eff->text()));
    m_psolve->m_indata.mrgn = convertToValues(static_cast<QString>(ui->Eff->text())); /* TMP after use real value */

    auto outPwr =
        [=](float mrg)
    {
            return ((m_psolve->m_indata.volt_out_one * m_psolve->m_indata.curr_out_one)
                    +(m_psolve->m_indata.volt_out_two * m_psolve->m_indata.curr_out_two)
                    +(m_psolve->m_indata.volt_out_three * m_psolve->m_indata.curr_out_three)
                    +(m_psolve->m_indata.volt_out_four * m_psolve->m_indata.curr_out_four)
                    *(m_psolve->m_indata.volt_out_aux * m_psolve->m_indata.curr_out_aux)) * mrg;
    };
    m_psolve->m_indata.power_out_max = outPwr(m_psolve->m_indata.mrgn);

    m_psolve->m_indata.refl_volt_max = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->ReflVoltage->text())));
    m_psolve->m_indata.voltage_spike = static_cast<int16_t>(convertToValues(static_cast<QString>(ui->VSpike->text())));
    m_psolve->m_indata.ripple_fact = convertToValues(static_cast<QString>(/*TMP*/ui->VSpike->text()));
    m_psolve->m_indata.eff_transf = convertToValues(static_cast<QString>(ui->EffTransf->text()));
    m_psolve->m_indata.volt_diode_drop_sec = convertToValues(static_cast<QString>(ui->VoltDropSec->text()));
    m_psolve->m_indata.volt_diode_drop_bridge = convertToValues(static_cast<QString>(ui->VoltBridgeDrop->text()));
    m_psolve->m_indata.leakage_induct = convertToValues(static_cast<QString>(ui->LeakageInduct->text()));

    ui->InductanceFact->setReadOnly(ui->ALUse->checkState() == Qt::Unchecked);
    ui->RGDiam->setReadOnly(ui->RGap->checkState() == Qt::Unchecked);
}

void FLySMPS::setInputNetwork()
{
    ui->DiodeCurrPeak->setNum(m_psolve->m_db->diode_peak_curr);
    ui->DiodeCurrRMS->setNum(m_psolve->m_db->diode_rms_curr);
    ui->DiodeCurrAVG->setNum(m_psolve->m_db->diode_avg_curr);
    ui->DiodeCurrRMSTot->setNum(m_psolve->m_db->diode_rms_curr_tot);
    ui->LoadCurrAVG->setNum(m_psolve->m_db->load_avg_curr);
    ui->DiodeCurrSlope->setNum(m_psolve->m_db->diode_curr_slope);
    ui->DiodeCondTime->setNum(m_psolve->m_db->diode_cond_time);
    ui->VoltMinPeakInput->setNum(m_psolve->m_db->in_min_rms_voltage);
    ui->VoltMaxPeakInput->setNum(m_psolve->m_db->in_max_rms_voltage);

    ui->DeltaT->setNum(m_psolve->m_bc->delta_t);
    ui->ChargT->setNum(m_psolve->m_bc->charg_time);
    ui->ILoadMax->setNum(m_psolve->m_bc->load_curr_max);
    ui->ILoadMin->setNum(m_psolve->m_bc->load_curr_min);
    ui->BulkCapacitance->setNum(m_psolve->m_bc->bcapacitor_value);
    ui->IBulkCapPeak->setNum(m_psolve->m_bc->bcapacitor_peak_curr);
    ui->IBulkCapRMS->setNum(m_psolve->m_bc->bcapacitor_rms_curr);
    ui->VoltDCMin->setNum(m_psolve->m_bc->input_dc_min_voltage);
    ui->MinInpVolt->setNum(m_psolve->m_bc->input_min_voltage);
}

void FLySMPS::initTransValues()
{
    m_psolve->m_ca.mag_flux_dens = convertToValues(static_cast<QString>(ui->InputBMax->text()));
    m_psolve->m_ca.win_util_factor = convertToValues(static_cast<QString>(ui->WinUtilFact->text()));
    m_psolve->m_ca.max_curr_dens = convertToValues(static_cast<QString>(ui->MaxCurrDens->text()));

    /** If use area product */
    if(ui->AEUse->isChecked()){
        m_psolve->m_fns = FBPT_NUM_SETTING::FBPT_CORE_AREA;
    }
    /** If use AL factor for calculate */
    else if(ui->ALUse->isChecked()){
        m_psolve->m_fns = FBPT_NUM_SETTING::FBPT_INDUCT_FACTOR;
        QString ind_fct;
        ui->InductanceFact->textEdited(ind_fct);
        m_psolve->m_cs.ind_fact = convertToValues(ind_fct);
        //TODO Check error value, use QValidator
    }
    /** If use maximum flux density */
    else if(ui->BMUse->isChecked()){
        m_psolve->m_fns = FBPT_NUM_SETTING::FBPT_FLUX_PEAK;
    }
}

void FLySMPS::setInitialiseTransProp()
{
    ui->MaxDutyCycle->setNum(m_psolve->m_ptpe->max_duty_cycle);
    ui->InputPWR->setNum(m_psolve->m_ptpe->inp_power);
    ui->InductPri->setNum(m_psolve->m_ptpe->primary_induct);

    ui->CurrPriAverage->setNum(m_psolve->m_ptpe->curr_primary_aver);
    ui->CurPriPkPk->setNum(m_psolve->m_ptpe->curr_primary_peak_peak);
    ui->CurPriMax->setNum(m_psolve->m_ptpe->curr_primary_peak);
    ui->CurPriValley->setNum(m_psolve->m_ptpe->curr_primary_valley);
    ui->CurPriRMS->setNum(m_psolve->m_ptpe->curr_primary_rms);
}

//TODO reimplement for use one check branch
void FLySMPS::initTransCoreValues()
{
    if(ui->SGap->isChecked()){
        m_psolve->m_fsag = FBPT_SHAPE_AIR_GAP::RECT_AIR_GAP;
    }
    else if(ui->RGap->isChecked()){
        m_psolve->m_fsag = FBPT_SHAPE_AIR_GAP::ROUND_AIR_GAP;
    }

    m_psolve->m_cs.core_cross_sect_area = convertToValues(static_cast<QString>(ui->AE->text()));
    m_psolve->m_cs.core_wind_area = convertToValues(static_cast<QString>(ui->WA->text()));
    m_psolve->m_cs.core_vol = convertToValues(static_cast<QString>(ui->VE->text()));
    m_psolve->m_cs.mean_leng_per_turn = convertToValues(static_cast<QString>(ui->MLT->text()));
    m_psolve->m_cs.mean_mag_path_leng = convertToValues(static_cast<QString>(ui->AE->text()));
    m_psolve->m_md.D = convertToValues(static_cast<QString>(ui->Dsize->text()));
    m_psolve->m_md.C = convertToValues(static_cast<QString>(ui->Csize->text()));
    m_psolve->m_md.F = convertToValues(static_cast<QString>(ui->Fsize->text()));
    m_psolve->m_md.E = convertToValues(static_cast<QString>(ui->Esize->text()));

    /** If use core with round central kern */
    if(m_psolve->m_fsag == FBPT_SHAPE_AIR_GAP::ROUND_AIR_GAP){
        QString diam_str;
        ui->RGDiam->textEdited(diam_str);
        m_psolve->m_md.Diam = convertToValues(diam_str);
        //TODO Check error value, use QValidator
    }
    else{
        m_psolve->m_md.Diam = 0.;
    }
}

void FLySMPS::setTransPrimaryProp()
{
    ui->WaAe->setNum(m_psolve->m_ptpe->core_area_product);
    ui->GeomCoeff->setNum(m_psolve->m_ptpe->core_geom_coeff);
    ui->PrimaryNum->setNum(m_psolve->m_ptpe->number_primary);
    ui->CurrDensity->setNum(m_psolve->m_ptpe->curr_dens);
    ui->LengAirGap->setNum(m_psolve->m_ptpe->length_air_gap);
    ui->FrigRluxCoeff->setNum(m_psolve->m_ptpe->fring_flux_fact);

    ui->ActPrimaryNum->setNum(m_psolve->m_ptpe->actual_num_primary);
    ui->ActBMax->setNum(m_psolve->m_ptpe->actual_flux_dens_peak);
    ui->ActReflVolt->setNum(m_psolve->m_ptpe->actual_volt_reflected);
    ui->ActDutyMax->setNum(m_psolve->m_ptpe->actual_max_duty_cycle);
}

void FLySMPS::initTransWireds()
{

}

void FLySMPS::setTransWiredProp()
{

}

void FLySMPS::initMosfetValues()
{
    m_psolve->m_mospr.m_vds;
    m_psolve->m_mospr.m_idr;
    m_psolve->m_mospr.m_qg;
    m_psolve->m_mospr.m_coss;
    m_psolve->m_mospr.m_rdson;

    m_psolve->m_ccsp.cl_first_out_volt = m_psolve->m_indata.volt_out_one;

    auto commTR =
            [=](double amdc, int32_t nump)
    {
        double n_frst = amdc/((1-amdc)*(m_psolve->m_indata.volt_out_one/
                                       (M_SQRT2*m_psolve->m_indata.input_volt_ac_min)));
        return static_cast<float>(nump/n_frst);
    };

    m_psolve->m_ccsp.cl_turn_rat = commTR(m_psolve->m_ptpe->actual_max_duty_cycle, m_psolve->m_ptpe->actual_num_primary);
    m_psolve->m_ccsp.cl_vol_rip;

    m_psolve->m_ccsp.cs_volt;
}

void FLySMPS::initOutDiodeValues()
{

}

void FLySMPS::initOutCapValues()
{
    /*m_psolve->m_indata.sec_voltage_ripple = convertToValues(static_cast<QString>(ui->));
    m_psolve->m_indata.sec_esr_perc = convertToValues(static_cast<QString>(ui->));
    m_psolve->m_indata.sec_crfq_value = convertToValues(static_cast<QString>(ui->));*/
}

double FLySMPS::convertToValues(const QString &input)
{
    QRegExp reg = QRegExp("^(.+)([pnμumKM])$");
    reg.indexIn(input);

    if(reg.captureCount() == 2)
    {
        QString strValue = reg.cap(1);
        QString unit = reg.cap(2);
        bool ok = false;
        double value = strValue.toDouble(&ok);

        if(!ok){return input.toDouble();}

        if(unit == "p"){
            return (value*1e-12);
        }
        else if(unit == "n"){
            return (value*1e-9);
        }
        else if(unit == "μ" || unit == "u"){
            return (value*1e-6);
        }
        else if(unit == "m"){
            return (value*1e-3);
        }
        else if(unit == "K"){
            return (value*1e3);
        }
        else if(unit == "M"){
            return (value*1e6);
        }
        return value;
    }
    return 0;
}
