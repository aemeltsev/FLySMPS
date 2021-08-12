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
    void setCoreAreaProp();

    void initTransCoreValues();
    void setTransPrimaryProp();

    void initTransWireds();
    void setTransWiredProp();

    void initMosfetValues();
    void setSolveMosfet();

    void setSolveOutDiode();
    void initOutCapValues();
    void setOutCap();

    void initOutFilter();
    void setSolveLCFilter();
    void setLCPlot();

    void initPowerStageModel();
    void setPowerStageModel();
    void setPowerStagePlot();

    void initOptoFeedbStage();
    void setOptoFeedbStage();
    void setOptoFeedbPlot();

    void setUpdateInputValues();

signals:
    void initTransValuesComplete();
    void initTransCoreValuesComplete();
    void initTransWiredsComplete();
    void initMosfetValuesComplete();
    void initOutCapValuesComplete();
    void initOutFilterComplete();
    void initPowerStageModelComplete();
    void initOptoFeedbStageComplete();

private:
    void initInputValues();
    double convertToValues(const QString& input);   

    QScopedPointer<Ui::FLySMPS> ui;
    QScopedPointer<PowSuppSolve> m_psolve;

    QList<QLabel*> d_out_one;
    QList<QLabel*> d_out_two;
    QList<QLabel*> d_out_three;
    QList<QLabel*> d_out_four;
    QList<QLabel*> d_out_aux;

    QList<QLabel*> cap_out_one;
    QList<QLabel*> cap_out_two;
    QList<QLabel*> cap_out_three;
    QList<QLabel*> cap_out_four;
    QList<QLabel*> cap_out_aux;

    QThread *m_psthread;
};

#endif // FLYSMPS_H
