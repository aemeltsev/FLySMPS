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

#ifndef OUTFILTER_H
#define OUTFILTER_H
#include <QtMath>
#include <QVector>
#include <QDebug>
#include <cstdint>

#define M_PI_DEG    180

class OutFilter : public QObject
{
    Q_OBJECT

signals:
    void arrayComplete();

public:
    /**
     * @brief OutFilter -
     * @param fr -
     * @param cap -
     */
    explicit OutFilter(int32_t fr, int32_t rload, QObject *parent = nullptr) noexcept;

    /**
     * @brief ofAngularCutFreq - Filter angular cut frequency
     * @return
     */
    inline double ofAngularCutFreq() const {return 2*M_PI*m_freq;}

    /**
     * @brief ofCapacitor - Capacitance in F
     * @return
     */
    inline double ofCapacitor() const {return 1./(M_SQRT2 * m_rload * ofAngularCutFreq());}

    /**
     * @brief ofInductor - Inductance in H
     * @return
     */
    inline double ofInductor() const {return  (m_rload * M_SQRT2)/(ofAngularCutFreq());}

    /******************************************************************/
    /**
     * @brief ofQualityFactor - Q factor
     * @return
     */
    inline double ofQualityFactor(){return m_rload * qSqrt(ofCapacitor()/ofInductor());}

    /**
     * @brief ofDampingRatio - The damping ratio(\zeta)
     * @return
     */
    inline double ofDampingRatio(){return ofInductor()/(2 * ofQualityFactor())/**1./(2 * ofQualityFactor())*/;}

    /**
     * @brief ofCutOffFreq - Cutoff Frequency
     * @return
     */
    inline double ofCutOffFreq(){return 1./(2*M_PI*qSqrt(ofCapacitor() * ofInductor()));}

    /**
     * @brief ofOutRipplVolt - Output ripple voltage
     * @return
     */
    inline double ofOutRipplVolt() const;

    /**
     * @brief ofPlotArray - Filling of data table
     * @param tabmap - ptr to map object
     * @param begin  - begin frequency point
     * @param end - end frequency point
     * @param step - frequency step
     */
    void ofPlotArray(QVector<double> &freq_vector, QVector<double> &mag_vector, QVector<double> &phase_vector, int32_t begin, int32_t end, int32_t step);

private:
    inline double ofTFMagnitude(const double freq);

    inline double ofTFMagnitudeGain(const double freq);

    inline double ofTFPhaseAng(const double freq);

    inline double ofTFphase(const double freq);

    int32_t m_freq=0;
    int32_t m_rload=0;
};

#endif // OUTFILTER_H
