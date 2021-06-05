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
