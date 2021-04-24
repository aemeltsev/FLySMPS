#include "FLySMPS.h"


FLySMPS::FLySMPS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FLySMPS),
    m_psolve(new PowSuppSolve)
{
    ui->setupUi(this);
    initInputValues();

    connect(ui->InpCalcPushButton, &QPushButton::clicked, m_psolve.data(), &PowSuppSolve::calcInputNetwork);

}

FLySMPS::~FLySMPS()
{
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

}

void FLySMPS::initTransValues()
{

}

void FLySMPS::initMosfetValues()
{

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
