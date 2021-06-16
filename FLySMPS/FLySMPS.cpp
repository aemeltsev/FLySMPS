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
                    +(m_psolve->m_indata.volt_out_aux * m_psolve->m_indata.curr_out_aux)) * mrg;
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

    QList<QLabel*> d_out_one;
    d_out_one << ui->Out1Pwr << ui->Out1Volt << ui->Out1TR << ui->Out1DRV << ui->Out1Diss;
    QList<QLabel*> d_out_two;
    d_out_two << ui->Out2Pwr << ui->Out2Volt << ui->Out2TR << ui->Out2DRV << ui->Out2Diss;
    QList<QLabel*> d_out_three;
    d_out_three << ui->Out3Pwr << ui->Out3Volt << ui->Out3TR << ui->Out3DRV << ui->Out3Diss;
    QList<QLabel*> d_out_four;
    d_out_four << ui->Out4Pwr << ui->Out4Volt << ui->Out4TR << ui->Out4DRV << ui->Out4Diss;
    QList<QLabel*> d_out_aux;
    d_out_aux << ui->AuxPwr << ui->AuxVolt << ui->AuxTR << ui->AuxDRV << ui->AuxDiss;

    QList<QLabel*> cap_out_one;
    cap_out_one << ui->Out1Cap << ui->Out1CapEsr << ui->Out1CurrRMS << ui->Out1CapZF << ui->Out1CapVRip << ui->Out1CapDiss;
    QList<QLabel*> cap_out_two;
    cap_out_two << ui->Out2Cap << ui->Out2CapEsr << ui->Out2CurrRMS << ui->Out2CapZF << ui->Out2CapVRip << ui->Out2CapDiss;
    QList<QLabel*> cap_out_three;
    cap_out_three << ui->Out3Cap << ui->Out3CapEsr << ui->Out3CurrRMS << ui->Out3CapZF << ui->Out3CapVRip << ui->Out3CapDiss;
    QList<QLabel*> cap_out_four;
    cap_out_four << ui->Out4Cap << ui->Out4CapEsr << ui->Out4CurrRMS << ui->Out4CapZF << ui->Out4CapVRip << ui->Out4CapDiss;
    QList<QLabel*> cap_out_aux;
    cap_out_aux << ui->AuxCap << ui->AuxCapEsr << ui->AuxCurrRMS << ui->AuxCapZF << ui->AuxCapVRip << ui->AuxCapDiss;
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
    m_psolve->m_psw.m_af[0] = convertToValues(static_cast<QString>(ui->AFNPm->text()));
    m_psolve->m_psw.m_af[1] = convertToValues(static_cast<QString>(ui->AFOut1->text()));;
    m_psolve->m_psw.m_af[2] = convertToValues(static_cast<QString>(ui->AFOut2->text()));;
    m_psolve->m_psw.m_af[3] = convertToValues(static_cast<QString>(ui->AFOut3->text()));;
    m_psolve->m_psw.m_af[4] = convertToValues(static_cast<QString>(ui->AFOut4->text()));;
    m_psolve->m_psw.m_af[5] = convertToValues(static_cast<QString>(ui->AFAux->text()));;

    m_psolve->m_psw.m_ins[0] = convertToValues(static_cast<QString>(ui->INSPm->text()));
    m_psolve->m_psw.m_ins[1] = convertToValues(static_cast<QString>(ui->INSOut1->text()));
    m_psolve->m_psw.m_ins[2] = convertToValues(static_cast<QString>(ui->INSOut2->text()));
    m_psolve->m_psw.m_ins[3] = convertToValues(static_cast<QString>(ui->INSOut3->text()));
    m_psolve->m_psw.m_ins[4] = convertToValues(static_cast<QString>(ui->INSOut4->text()));
    m_psolve->m_psw.m_ins[5] = convertToValues(static_cast<QString>(ui->INSAux->text()));

    m_psolve->m_psw.m_fcu = convertToValues(static_cast<QString>(ui->Fcu->text()));
    m_psolve->m_psw.m_mcd = convertToValues(static_cast<QString>(ui->InM->text()));

}

void FLySMPS::setTransWiredProp()
{
    ui->Out1ISMax->setNum(m_psolve->m_ptsw->out_one_wind["JSP"]);
    ui->Out1ISRMS->setNum(m_psolve->m_ptsw->out_one_wind["JSRMS"]);
    ui->Out1NSec->setNum(m_psolve->m_ptsw->out_one_wind["NSEC"]);
    ui->Out1ANS->setNum(m_psolve->m_ptsw->out_one_wind["ANS"]);
    ui->Out1AWGNS->setNum(m_psolve->m_ptsw->out_one_wind["AWGNS"]);
    ui->Out1DS->setNum(m_psolve->m_ptsw->out_one_wind["DS"]);
    ui->Out1ECA->setNum(m_psolve->m_ptsw->out_one_wind["ECA"]);
    ui->Out1JS->setNum(m_psolve->m_ptsw->out_one_wind["JS"]);
    ui->Out1OD->setNum(m_psolve->m_ptsw->out_one_wind["OD"]);
    ui->Out1NTL->setNum(m_psolve->m_ptsw->out_one_wind["NTL"]);
    ui->Out1LN->setNum(m_psolve->m_ptsw->out_one_wind["LN"]);

    ui->Out2ISMax->setNum(m_psolve->m_ptsw->out_two_wind["JSP"]);
    ui->Out2ISRMS->setNum(m_psolve->m_ptsw->out_two_wind["JSRMS"]);
    ui->Out2NSec->setNum(m_psolve->m_ptsw->out_two_wind["NSEC"]);
    ui->Out2ANS->setNum(m_psolve->m_ptsw->out_two_wind["ANS"]);
    ui->Out2AWGNS->setNum(m_psolve->m_ptsw->out_two_wind["AWGNS"]);
    ui->Out2DS->setNum(m_psolve->m_ptsw->out_two_wind["DS"]);
    ui->Out2ECA->setNum(m_psolve->m_ptsw->out_two_wind["ECA"]);
    ui->Out2JS->setNum(m_psolve->m_ptsw->out_two_wind["JS"]);
    ui->Out2OD->setNum(m_psolve->m_ptsw->out_two_wind["OD"]);
    ui->Out2NTL->setNum(m_psolve->m_ptsw->out_two_wind["NTL"]);
    ui->Out2LN->setNum(m_psolve->m_ptsw->out_two_wind["LN"]);

    ui->Out3ISMax->setNum(m_psolve->m_ptsw->out_three_wind["JSP"]);
    ui->Out3ISRMS->setNum(m_psolve->m_ptsw->out_three_wind["JSRMS"]);
    ui->Out3NSec->setNum(m_psolve->m_ptsw->out_three_wind["NSEC"]);
    ui->Out3ANS->setNum(m_psolve->m_ptsw->out_three_wind["ANS"]);
    ui->Out3AWGNS->setNum(m_psolve->m_ptsw->out_three_wind["AWGNS"]);
    ui->Out3DS->setNum(m_psolve->m_ptsw->out_three_wind["DS"]);
    ui->Out3ECA->setNum(m_psolve->m_ptsw->out_three_wind["ECA"]);
    ui->Out3JS->setNum(m_psolve->m_ptsw->out_three_wind["JS"]);
    ui->Out3OD->setNum(m_psolve->m_ptsw->out_three_wind["OD"]);
    ui->Out3NTL->setNum(m_psolve->m_ptsw->out_three_wind["NTL"]);
    ui->Out3LN->setNum(m_psolve->m_ptsw->out_three_wind["LN"]);

    ui->Out4ISMax->setNum(m_psolve->m_ptsw->out_four_wind["JSP"]);
    ui->Out4ISRMS->setNum(m_psolve->m_ptsw->out_four_wind["JSRMS"]);
    ui->Out4NSec->setNum(m_psolve->m_ptsw->out_four_wind["NSEC"]);
    ui->Out4ANS->setNum(m_psolve->m_ptsw->out_four_wind["ANS"]);
    ui->Out4AWGNS->setNum(m_psolve->m_ptsw->out_four_wind["AWGNS"]);
    ui->Out4DS->setNum(m_psolve->m_ptsw->out_four_wind["DS"]);
    ui->Out4ECA->setNum(m_psolve->m_ptsw->out_four_wind["ECA"]);
    ui->Out4JS->setNum(m_psolve->m_ptsw->out_four_wind["JS"]);
    ui->Out4OD->setNum(m_psolve->m_ptsw->out_four_wind["OD"]);
    ui->Out4NTL->setNum(m_psolve->m_ptsw->out_four_wind["NTL"]);
    ui->Out4LN->setNum(m_psolve->m_ptsw->out_four_wind["LN"]);

    ui->AuxN->setNum(m_psolve->m_ptsw->out_aux_wind["NSEC"]);
    ui->AuxAN->setNum(m_psolve->m_ptsw->out_aux_wind["ANS"]);
    ui->AuxAWGN->setNum(m_psolve->m_ptsw->out_aux_wind["AWGNS"]);
    ui->AuxD->setNum(m_psolve->m_ptsw->out_aux_wind["DS"]);
    ui->AuxECA->setNum(m_psolve->m_ptsw->out_aux_wind["ECA"]);
    ui->AuxOD->setNum(m_psolve->m_ptsw->out_aux_wind["OD"]);
    ui->AuxNTL->setNum(m_psolve->m_ptsw->out_aux_wind["NTL"]);

    ui->PrimAP->setNum(m_psolve->m_ptsw->out_aux_wind["AP"]);
    ui->PrimAWGP->setNum(m_psolve->m_ptsw->out_aux_wind["AWGP"]);
    ui->PrimDP->setNum(m_psolve->m_ptsw->out_aux_wind["DP"]);
    ui->PrimECA->setNum(m_psolve->m_ptsw->out_aux_wind["ECA"]);
    ui->PrimJP->setNum(m_psolve->m_ptsw->out_aux_wind["JP"]);
    ui->PrimOD->setNum(m_psolve->m_ptsw->out_aux_wind["OD"]);
    ui->PrimNTL->setNum(m_psolve->m_ptsw->out_aux_wind["NTL"]);
    ui->PrimLN->setNum(m_psolve->m_ptsw->out_aux_wind["LN"]);
}

void FLySMPS::initMosfetValues()
{
    m_psolve->m_mospr.m_vgs = convertToValues(static_cast<QString>(ui->VGS->text()));
    m_psolve->m_mospr.m_idr = convertToValues(static_cast<QString>(ui->CurrDrv->text()));
    m_psolve->m_mospr.m_qg = convertToValues(static_cast<QString>(ui->QGate->text()));
    m_psolve->m_mospr.m_coss = convertToValues(static_cast<QString>(ui->COss->text()));
    m_psolve->m_mospr.m_rdson = convertToValues(static_cast<QString>(ui->RdsOn->text()));

    m_psolve->m_ccsp.cl_first_out_volt = m_psolve->m_indata.volt_out_one;

    // Determine the turns ratio
    // See Ayachit A.-Magnetising inductance of multiple-output flyback dc–dc convertor for dcm.
    auto commTR =
            [=](double amdc, int32_t nump)
    {
        double n_frst = amdc/((1-amdc)*(m_psolve->m_indata.volt_out_one/
                                       (M_SQRT2*m_psolve->m_indata.input_volt_ac_min)));
        return static_cast<float>(nump/n_frst);
    };

    m_psolve->m_ccsp.cl_turn_rat = commTR(m_psolve->m_ptpe->actual_max_duty_cycle, m_psolve->m_ptpe->actual_num_primary);
    m_psolve->m_ccsp.cl_vol_rip = convertToValues(static_cast<QString>(ui->SnubbVoltRipp->text()));

    m_psolve->m_ccsp.cs_volt = convertToValues(static_cast<QString>(ui->CSVolt->text()));
}

void FLySMPS::setSolveMosfet()
{
    /**Todo move to transformer calc section*/
    ui->VDSmax->setNum(m_psolve->m_pm->mosfet_voltage_max);
    ui->VDSnom->setNum(m_psolve->m_pm->mosfet_voltage_nom);
    ui->IDSmax->setNum(m_psolve->m_pm->mosfet_ds_curr);
    /***/

    ui->Toff->setNum(m_psolve->m_pm->mosfet_off_time);
    ui->Ton->setNum(m_psolve->m_pm->mosfet_on_time);
    ui->Ttot->setNum(m_psolve->m_pm->mosfet_sw_tot);
    ui->Trise_fall->setNum(m_psolve->m_pm->mosfet_rise_time);

    ui->MosCondL->setNum(m_psolve->m_pm->mosfet_conduct_loss);
    ui->MosDL->setNum(m_psolve->m_pm->mosfet_drive_loss);
    ui->MosSL->setNum(m_psolve->m_pm->mosfet_switch_loss);
    ui->MosCapL->setNum(m_psolve->m_pm->mosfet_capacit_loss);
    ui->MosTL->setNum(m_psolve->m_pm->mosfet_total_loss);

    ui->SnubbVM->setNum(m_psolve->m_pm->snubber_voltage_max);
    ui->SnubbR->setNum(m_psolve->m_pm->snubber_res_value);
    ui->SnubbC->setNum(m_psolve->m_pm->snubber_cap_value);
    ui->SnubbPL->setNum(m_psolve->m_pm->snubber_pwr_diss);

    ui->CurrR->setNum(m_psolve->m_pm->curr_sense_res);
    ui->CurrRL->setNum(m_psolve->m_pm->curr_sense_res_loss);
}

void FLySMPS::setSolveOutDiode()
{
    int16_t a=0, b=0, c=0, d=0, e=0; //counters

    for(auto one_itr = m_psolve->m_fod->out_diode_first.constBegin();
        one_itr != m_psolve->m_fod->out_diode_first.constEnd();
        ++a, ++one_itr)
    {
        d_out_one[a]->setNum(one_itr.value());
    }

    for(auto two_itr = m_psolve->m_fod->out_diode_sec.constBegin();
        two_itr != m_psolve->m_fod->out_diode_sec.constEnd();
        ++b, ++two_itr)
    {
        d_out_two[b]->setNum(two_itr.value());
    }

    for(auto three_itr = m_psolve->m_fod->out_diode_thrid.constBegin();
        three_itr != m_psolve->m_fod->out_diode_thrid.constEnd();
        ++b, ++three_itr)
    {
        d_out_three[c]->setNum(three_itr.value());
    }

    for(auto four_itr = m_psolve->m_fod->out_diode_four.constBegin();
        four_itr != m_psolve->m_fod->out_diode_four.constEnd();
        ++b, ++four_itr)
    {
        d_out_four[d]->setNum(four_itr.value());
    }

    for(auto aux_itr = m_psolve->m_fod->out_diode_aux.constBegin();
        aux_itr != m_psolve->m_fod->out_diode_aux.constEnd();
        ++b, ++aux_itr)
    {
        d_out_aux[e]->setNum(aux_itr.value());
    }
}

void FLySMPS::initOutCapValues()
{
    m_psolve->m_cop[0].co_volts_rippl = convertToValues(static_cast<QString>(ui->Out1VRip->text()));
    m_psolve->m_cop[0].co_esr_perc = convertToValues(static_cast<QString>(ui->Out1ESRPerc->text()));
    m_psolve->m_cop[0].co_cros_frq_start_val = convertToValues(static_cast<QString>(ui->Out1ZFC->text()));
    m_psolve->m_cop[0].co_volts_out = m_psolve->m_indata.volt_out_one;
    m_psolve->m_cop[0].co_curr_peak_out = m_psolve->m_indata.curr_out_one;

    m_psolve->m_cop[1].co_volts_rippl = convertToValues(static_cast<QString>(ui->Out2VRip->text()));
    m_psolve->m_cop[1].co_esr_perc = convertToValues(static_cast<QString>(ui->Out2ESRPerc->text()));
    m_psolve->m_cop[1].co_cros_frq_start_val = convertToValues(static_cast<QString>(ui->Out2ZFC->text()));
    m_psolve->m_cop[1].co_volts_out = m_psolve->m_indata.volt_out_two;
    m_psolve->m_cop[1].co_curr_peak_out = m_psolve->m_indata.curr_out_two;

    m_psolve->m_cop[2].co_volts_rippl = convertToValues(static_cast<QString>(ui->Out3VRip->text()));
    m_psolve->m_cop[2].co_esr_perc = convertToValues(static_cast<QString>(ui->Out3ESRPerc->text()));
    m_psolve->m_cop[2].co_cros_frq_start_val = convertToValues(static_cast<QString>(ui->Out3ZFC->text()));
    m_psolve->m_cop[2].co_volts_out = m_psolve->m_indata.volt_out_three;
    m_psolve->m_cop[2].co_curr_peak_out = m_psolve->m_indata.curr_out_three;

    m_psolve->m_cop[3].co_volts_rippl = convertToValues(static_cast<QString>(ui->Out4VRip->text()));
    m_psolve->m_cop[3].co_esr_perc = convertToValues(static_cast<QString>(ui->Out4ESRPerc->text()));
    m_psolve->m_cop[3].co_cros_frq_start_val = convertToValues(static_cast<QString>(ui->Out4ZFC->text()));
    m_psolve->m_cop[3].co_volts_out = m_psolve->m_indata.volt_out_four;
    m_psolve->m_cop[3].co_curr_peak_out = m_psolve->m_indata.curr_out_four;

    m_psolve->m_cop[4].co_volts_rippl = convertToValues(static_cast<QString>(ui->AuxVRip->text()));
    m_psolve->m_cop[4].co_esr_perc = convertToValues(static_cast<QString>(ui->AuxESRPerc->text()));
    m_psolve->m_cop[4].co_cros_frq_start_val = convertToValues(static_cast<QString>(ui->AuxZFC->text()));
    m_psolve->m_cop[4].co_volts_out = m_psolve->m_indata.volt_out_aux;
    m_psolve->m_cop[4].co_curr_peak_out = m_psolve->m_indata.curr_out_aux;

}

void FLySMPS::setOutCap()
{
    int16_t a=0, b=0, c=0, d=0, e=0; //counters

    for(auto one_itr = m_psolve->m_foc->out_cap_first.constBegin();
        one_itr != m_psolve->m_foc->out_cap_first.constEnd();
        ++a, ++one_itr)
    {
        cap_out_one[a]->setNum(one_itr.value());
    }

    for(auto two_itr = m_psolve->m_foc->out_cap_sec.constBegin();
        two_itr != m_psolve->m_foc->out_cap_sec.constEnd();
        ++b, ++two_itr)
    {
        cap_out_two[b]->setNum(two_itr.value());
    }

    for(auto three_itr = m_psolve->m_foc->out_cap_thrid.constBegin();
        three_itr != m_psolve->m_foc->out_cap_thrid.constEnd();
        ++b, ++three_itr)
    {
        cap_out_three[c]->setNum(three_itr.value());
    }

    for(auto four_itr = m_psolve->m_foc->out_cap_four.constBegin();
        four_itr != m_psolve->m_foc->out_cap_four.constEnd();
        ++b, ++four_itr)
    {
        cap_out_four[d]->setNum(four_itr.value());
    }

    for(auto aux_itr = m_psolve->m_foc->out_cap_aux.constBegin();
        aux_itr != m_psolve->m_foc->out_cap_aux.constEnd();
        ++b, ++aux_itr)
    {
        cap_out_aux[e]->setNum(aux_itr.value());
    }
}

void FLySMPS::initOutFilter()
{
    m_psolve->m_of->angular_cut_freq = convertToValues(static_cast<QString>(ui->LCF_Freq->text()));
    m_psolve->m_of->load_resistance = convertToValues(static_cast<QString>(ui->LCF_ResLoad->text()));
}

void FLySMPS::setSolveLCFilter()
{
    //todo
    ui->LCAngCutFreq->setNum(1);
    ui->LCInd->setNum(1);
    ui->LCCap->setNum(1);
    ui->LCQual->setNum(1);
    ui->LCDamp->setNum(1);
    ui->LCCutFreq->setNum(1);
    ui->LCOutRippVolt->setNum(1);
}

void FLySMPS::setLCPlot()
{
    //add two new graphs and set their look:
    ui->LCFilterGraph->clearGraphs();

    ui->LCFilterGraph->addGraph(ui->LCFilterGraph->xAxis, ui->LCFilterGraph->yAxis);
    ui->LCFilterGraph->graph(0)->setPen(QPen(Qt::blue));

    ui->LCFilterGraph->addGraph(ui->LCFilterGraph->xAxis2, ui->LCFilterGraph->yAxis2);
    ui->LCFilterGraph->graph(1)->setPen(QPen(Qt::red));

    //configure right and top axis to show ticks but no labels:
    ui->LCFilterGraph->xAxis2->setVisible(true);
    ui->LCFilterGraph->yAxis2->setVisible(true);

    //pass data points to graphs:
    ui->LCFilterGraph->graph(0)->setData(m_psolve->m_of->of_freq_array, m_psolve->m_of->of_magnitude_array);
    ui->LCFilterGraph->graph(1)->setData(m_psolve->m_of->of_freq_array, m_psolve->m_of->of_phase_array);
    ui->LCFilterGraph->replot();

    //give the axis some labels:
    ui->LCFilterGraph->xAxis->setLabel("Freq. Hz");
    ui->LCFilterGraph->yAxis->setLabel("Mag. dB");
    ui->LCFilterGraph->yAxis2->setLabel("Deg. ");

    //set axes ranges, so we see all data:
    ui->LCFilterGraph->yAxis->setRange(-27, 3);
    ui->LCFilterGraph->xAxis->setRange(0, 1000000);
    ui->LCFilterGraph->yAxis2->setRange(0, -90);
    ui->LCFilterGraph->xAxis2->setRange(0, 1000000);

    //
    m_psolve->m_of->of_freq_array.clear();
    m_psolve->m_of->of_magnitude_array.clear();
    m_psolve->m_of->of_phase_array.clear();
}

void FLySMPS::initPowerStageModel()
{
    // External ramp slope compensation(S_e)
    // Basso C.-The TL431 in Switch-Mode Power Supplies loops: part V
    auto rsc = [=](int16_t p_vout, float p_diode_drop, float p_rsense,
                     float p_turns_ratio, double p_prim_ind)

    {
        return 0.5 * ((p_vout + p_diode_drop)/(p_turns_ratio * p_prim_ind) * p_rsense);
    };

    // Determine the turns ratio
    // See Ayachit A.-Magnetising inductance of multiple-output flyback dc–dc convertor for dcm.
    auto commTR =
            [=](double amdc, int32_t nump)
    {
        double n_frst = amdc/((1-amdc)*(m_psolve->m_indata.volt_out_one/
                                       (M_SQRT2*m_psolve->m_indata.input_volt_ac_min)));
        return static_cast<float>(nump/n_frst);
    };

    m_psolve->m_ssm.input_voltage = m_psolve->m_indata.input_volt_ac_min;
    m_psolve->m_ssm.freq_switch = m_psolve->m_indata.freq_switch;
    m_psolve->m_ssm.actual_duty = m_psolve->m_ptpe->actual_max_duty_cycle;
    m_psolve->m_ssm.primary_ind = m_psolve->m_ptpe->primary_induct;
    m_psolve->m_ssm.res_sense = m_psolve->m_pm->curr_sense_res;
    m_psolve->m_ssm.output_voltage = m_psolve->m_indata.volt_out_one;
    m_psolve->m_ssm.output_full_load_res = m_psolve->m_indata.volt_out_one/m_psolve->m_indata.curr_out_one;
    m_psolve->m_ssm.turn_ratio = commTR(m_psolve->m_ptpe->actual_max_duty_cycle, m_psolve->m_ptpe->actual_num_primary);
    m_psolve->m_ssm.output_cap = m_psolve->m_foc->out_cap_first["CVO"];
    m_psolve->m_ssm.output_cap_esr = m_psolve->m_foc->out_cap_first["CESRO"];
    m_psolve->m_ssm.sawvolt = rsc(m_psolve->m_indata.volt_out_one,
                                  m_psolve->m_indata.volt_diode_drop_sec,
                                  m_psolve->m_pm->curr_sense_res,
                                  commTR(m_psolve->m_ptpe->actual_max_duty_cycle, m_psolve->m_ptpe->actual_num_primary),
                                  m_psolve->m_ptpe->primary_induct);
}

void FLySMPS::setPowerStageModel()
{
    ui->PSMFz1->setNum(m_psolve->m_pssm->ps_zero_one);
    ui->PSMFp1->setNum(m_psolve->m_pssm->ps_pole_one);
    ui->PSMFz2dcm->setNum(m_psolve->m_pssm->ps_dcm_zero_two);
    ui->PSMFp2dcm->setNum(m_psolve->m_pssm->ps_dcm_pole_two);
    ui->PSMFz2ccm->setNum(m_psolve->m_pssm->ps_ccm_zero_two);
    ui->PSMFp2ccm->setNum(m_psolve->m_pssm->ps_ccm_pole_two);
    ui->PSMGf->setNum(m_psolve->m_pssm->ps_gain_cmc_mod);
}

void FLySMPS::setPowerStagePlot()
{
    ui->PSMGraph->clearGraphs();

    ui->PSMGraph->addGraph(ui->PSMGraph->xAxis, ui->PSMGraph->yAxis);
    ui->PSMGraph->graph(0)->setPen(QPen(Qt::blue));

    ui->PSMGraph->addGraph(ui->PSMGraph->xAxis2, ui->PSMGraph->yAxis2);
    ui->PSMGraph->graph(1)->setPen(QPen(Qt::red));

    //configure right and top axis to show ticks but no labels:
    ui->PSMGraph->xAxis2->setVisible(true);
    ui->PSMGraph->yAxis2->setVisible(true);

    //pass data points to graphs:
    ui->PSMGraph->graph(0)->setData(m_psolve->m_pssm->ps_freq_array, m_psolve->m_pssm->ps_magnitude_array);
    ui->PSMGraph->graph(1)->setData(m_psolve->m_of->of_freq_array, m_psolve->m_pssm->ps_phase_array);
    ui->PSMGraph->replot();

    //give the axis some labels:
    ui->PSMGraph->xAxis->setLabel("Freq. Hz");
    ui->PSMGraph->yAxis->setLabel("Mag. dB");
    ui->PSMGraph->yAxis2->setLabel("Deg. ");

    //set axes ranges, so we see all data:
    ui->PSMGraph->yAxis->setRange(-27, 3);
    ui->PSMGraph->xAxis->setRange(0, 1000000);
    ui->PSMGraph->yAxis2->setRange(0, -90);
    ui->PSMGraph->xAxis2->setRange(0, 1000000);

    //
    m_psolve->m_pssm->ps_freq_array.clear();
    m_psolve->m_pssm->ps_magnitude_array.clear();
    m_psolve->m_pssm->ps_phase_array.clear();

}

void FLySMPS::initOptoFeedbStage()
{
    m_psolve->m_fc.out_voltage = m_psolve->m_indata.volt_out_one;
    m_psolve->m_fc.out_current = m_psolve->m_indata.curr_out_one;
    m_psolve->m_fc.res_pull_up = convertToValues(static_cast<QString>(ui->ResPullUp->text()));
    m_psolve->m_fc.res_down = convertToValues(static_cast<QString>(ui->ResDown->text()));
    m_psolve->m_fc.phase_shift = convertToValues(static_cast<QString>(ui->PhaseMarg->text()));
    m_psolve->m_fc.amp_gaim_marg = convertToValues(static_cast<QString>(ui->GainMarg->text()));
    m_psolve->m_fc.opto_ctr = convertToValues(static_cast<QString>(ui->OptoCTR->text()));
    m_psolve->m_fc.freq_sw = m_psolve->m_indata.freq_switch;
    m_psolve->m_fc.opto_inner_cap = convertToValues(static_cast<QString>(ui->OptoInnerCap->text()));

    m_psolve->m_rs.inp_voltage = m_psolve->m_indata.input_volt_ac_min;
    m_psolve->m_rs.prim_turns = m_psolve->m_ptpe->actual_num_primary;
    m_psolve->m_rs.sec_turns_to_control = m_psolve->m_ptsw->out_one_wind["NSEC"];
    m_psolve->m_rs.actual_duty = m_psolve->m_ptpe->actual_max_duty_cycle;
    m_psolve->m_rs.out_pwr_tot = m_psolve->m_indata.power_out_max;
    m_psolve->m_rs.primary_ind = m_psolve->m_ptpe->primary_induct;
    m_psolve->m_rs.res_sense = m_psolve->m_pm->curr_sense_res;

    m_psolve->m_lc.lcf_ind = m_psolve->m_of->inductor;
    m_psolve->m_lc.lcf_cap = m_psolve->m_of->capacitor;
    m_psolve->m_lc.lcf_cap_esr = convertToValues(static_cast<QString>(ui->CapFilterESR->text()));
}

void FLySMPS::setOptoFeedbStage()
{
    ui->ResLed->setNum(m_psolve->m_ofs->of_opto_led_res);
    ui->ResBias->setNum(m_psolve->m_ofs->of_opto_bias_res);
    ui->ResUp->setNum(m_psolve->m_ofs->of_up_divide_res);

    ui->Quality->setNum(m_psolve->m_ofs->of_quality);
    ui->SE->setNum(m_psolve->m_ofs->of_ext_ramp_slope);
    ui->SN->setNum(m_psolve->m_ofs->of_ind_on_slope);
    ui->FCross->setNum(m_psolve->m_ofs->of_freq_cross_sect);
    ui->Fzero->setNum(m_psolve->m_ofs->of_zero);
    ui->Fpole->setNum(m_psolve->m_ofs->of_pole);

    ui->CapOpto->setNum(m_psolve->m_ofs->of_cap_opto);
    ui->ResZero->setNum(m_psolve->m_ofs->of_res_err_amp);
    ui->CapZero->setNum(m_psolve->m_ofs->of_cap_err_amp);
}

void FLySMPS::setOptoFeedbPlot()
{
    ui->OptoGraph->clearGraphs();

    ui->OptoGraph->addGraph(ui->OptoGraph->xAxis, ui->OptoGraph->yAxis);
    ui->OptoGraph->graph(0)->setPen(QPen(Qt::blue));

    ui->OptoGraph->addGraph(ui->OptoGraph->xAxis2, ui->OptoGraph->yAxis2);
    ui->OptoGraph->graph(1)->setPen(QPen(Qt::red));

    //configure right and top axis to show ticks but no labels:
    ui->OptoGraph->xAxis2->setVisible(true);
    ui->OptoGraph->yAxis2->setVisible(true);

    //pass data points to graphs:
    ui->OptoGraph->graph(0)->setData(m_psolve->m_ofs->of_freq_array, m_psolve->m_ofs->of_magnitude_array);
    ui->OptoGraph->graph(1)->setData(m_psolve->m_ofs->of_freq_array, m_psolve->m_ofs->of_phase_array);
    ui->OptoGraph->replot();

    //give the axis some labels:
    ui->OptoGraph->xAxis->setLabel("Freq. Hz");
    ui->OptoGraph->yAxis->setLabel("Mag. dB");
    ui->OptoGraph->yAxis2->setLabel("Deg. ");

    //set axes ranges, so we see all data:
    ui->OptoGraph->yAxis->setRange(-27, 3);
    ui->OptoGraph->xAxis->setRange(0, 1000000);
    ui->OptoGraph->yAxis2->setRange(0, -90);
    ui->OptoGraph->xAxis2->setRange(0, 1000000);

    //
    m_psolve->m_ofs->of_freq_array.clear();
    m_psolve->m_ofs->of_magnitude_array.clear();
    m_psolve->m_ofs->of_phase_array.clear();
}

void FLySMPS::setUpdateInputValues()
{
    //todo
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
