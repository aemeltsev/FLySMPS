#include "FLySMPS.h"
#include "ui_FLySMPS.h"
#include "diodebridge.h"
#include "bulkcap.h"

FLySMPS::FLySMPS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FLySMPS)
{
    ui->setupUi(this);
}

FLySMPS::~FLySMPS()
{
    delete ui;
}
