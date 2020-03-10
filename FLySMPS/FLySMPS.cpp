#include "FLySMPS.h"
#include "ui_FLySMPS.h"

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
