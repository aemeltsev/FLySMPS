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
#include <QtGlobal>
#include <QThread>
#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#include <QRegExp>
#else
#include <QRegularExpression>
#endif

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
    void setSolveLCFilter(QHash<QString, double> h_data);
    void setLCPlot(QVector<double> mg_data, QVector<double> ph_data);

    void initPowerStageModel();
    void setPowerStageModel(QHash<QString, double> h_data);
    void setPowerStagePlot(QVector<double> mg_data, QVector<double> ph_data);

    void initOptoFeedbStage();
    void setOptoFeedbStage(QHash<QString, double> h_data);
    void setOptoFeedbPlot(QVector<double> mg_data, QVector<double> ph_data);

    void setUpdateInputValues();
    //void checkCorrect(const QString &text);

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
    void initOutDCData();
    void initInputValues();
    void initLCPlot();
    void initFCPlot();
    void initSSMplot();
    double convertToValues(const QString& input);
    void updateVCData(const QString& input, bool chkval, bool err = false, int16_t vo=0, float io=0.0);
    double outPwr(const float mrg);

    QScopedPointer<Ui::FLySMPS> ui;
    QScopedPointer<PowSuppSolve> m_psolve;
    QThread* m_sthread;

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
};
#endif // FLYSMPS_H
