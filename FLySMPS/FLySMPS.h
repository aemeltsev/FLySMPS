#ifndef FLYSMPS_H
#define FLYSMPS_H

#include <QtWidgets>
#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include "powsuppsolve.h"
#include "qcustomplot.h"

#include "ui_FLySMPS.h"

namespace Ui {
class FLySMPS;
}

class FLySMPS : public QMainWindow
{
    Q_OBJECT

public:
    explicit FLySMPS(QWidget *parent = nullptr);
    ~FLySMPS();

private slots:

    void setInputNetwork();

    void initTransValues();
    void setInitialiseTransProp();

    void initTransCoreValues();
    void setTransPrimaryProp();

    void initTransWireds();
    void setTransWiredProp();

    void initMosfetValues();
    void setSolveMosfet();

    void initOutDiodeValues();
    void setSolveOutDiode();

    void initOutCapValues();
    void setOutCap();

    void initOutFilter();
    void setSolveLCFilter();

    void setLCPlotMag();
    void setLCPlotPhase();

    void setPowerStageModel();
    void setPowerStageMag();
    void setPowerStagePhase();
    void setOptoFeedbStage();
    void setOptoFeedbMag();
    void setOptoFeedbPhase();
    void setUpdateInputValues();

private:
    void initInputValues();
    double convertToValues(const QString& input);   

    QScopedPointer<Ui::FLySMPS> ui;
    QScopedPointer<PowSuppSolve> m_psolve;

    QThread *m_psworker;
};

#endif // FLYSMPS_H
