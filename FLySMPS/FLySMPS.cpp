#include "FLySMPS.h"


FLySMPS::FLySMPS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FLySMPS),
    m_psolve(new PowSuppSolve)
{
    ui->setupUi(this);
    initInputValues();
}

FLySMPS::~FLySMPS()
{
}

void FLySMPS::initInputValues()
{

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
