#ifndef FLYSMPS_H
#define FLYSMPS_H

#include <QtWidgets>
#include <QMainWindow>
#include <QDebug>
#include "powsuppsolve.h"
#include "structdata.h"

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

private:
    void initInputValues();
    void setSolveInputDiode();
    void setSolveBulkCap();
    void initTransValues();
    void setSolveTransPrimaryElecrt();
    void setSolveTransSecondWired();
    void initMosfetValues();
    void initOutDiodeValues();
    void initOutCapValues();
    void setSolveMosfet();
    void setSolveOutDiode();
    void setOutCap();
    void initOutFilter();
    void setSolveLCFilter();
    void setLCPlotMag();
    void setLCPlotPhase();
    void setPowerStageModel();
    void setPowerStageMag();
    void setPowerStagePhase();
    void setOptoFedbStage();
    void setOptoFedbMag();
    void setOptoFedbPhase();
    void setUpdateInputValues();
    double convertToValues(const QString& input);

    QScopedPointer<Ui::FLySMPS> ui;
    QScopedPointer<PowSuppSolve> m_psolve;
    QScopedPointer<indata::InputValue> m_invalues;
};

#endif // FLYSMPS_H
