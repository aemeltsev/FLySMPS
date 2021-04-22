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
