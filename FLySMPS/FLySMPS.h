#ifndef FLYSMPS_H
#define FLYSMPS_H

#include <QMainWindow>
#include <QDebug>

#include "ui_FLySMPS.h"
#include "LoggingCategories.h"
#include "structdata.h"
#include "diodebridge.h"
#include "bulkcap.h"
#include "fbptransformer.h"
#include "swmosfet.h"
#include "diodeout.h"
#include "capout.h"
#include "outfilter.h"

namespace Ui {
class FLySMPS;
}

class FLySMPS : public QMainWindow
{
    Q_OBJECT

public:
    explicit FLySMPS(QWidget *parent = nullptr);
    ~FLySMPS();

    void ReadInputData();
    void PreCalc();


private:
    Ui::FLySMPS *ui;

    InputValue ivl;
    DBridge dbr;
    BCap bcp;
    FBPT fbpt;
    PMosfet pmsf;

//public slots:
    //void PressCalcBTN();
};

#endif // FLYSMPS_H
