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
#include <cstdint>

#define M_PI_DEG    180

class OutFilter
{
public:
        /**
         * @brief OutFilter -
         * @param fr -
         * @param cap -
         */
        OutFilter(int16_t fr, int16_t rload):
            m_freq(fr), m_rload(rload)
        {}

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
        inline double ofDampingRatio(){return 1./(2 * ofQualityFactor());}
        /**
         * @brief ofCutOffFreq - Cutoff Frequency
         * @return
         */
        inline double ofCutOffFreq(){return 1./(2*M_PI*qSqrt(ofCapacitor() * ofInductor()));}
        /**
         * @brief ofOutRipplVolt - Output ripple voltage
         * @return
         */
        inline double ofOutRipplVolt()
        {
            return (1./(ofAngularCutFreq()*ofCapacitor()))/((1./(ofAngularCutFreq()*ofCapacitor()))+(ofAngularCutFreq()*ofInductor()));
        }

        /**
         * @brief ofPlotArray - Filling of data table
         * @param tabmap - ptr to map object
         * @param begin  - begin frequency point
         * @param end - end frequency point
         * @param step - frequency step
         */
        void ofPlotArray(QVector<int32_t> &freq_vector, QVector<double> &mag_vector, QVector<double> &phase_vector, int32_t begin, int32_t end, int32_t step)
        {
            for(int32_t ind=begin; ind<end; ind+=step)
            {
                freq_vector.push_back(ind);
                mag_vector.push_back(ofTFMagnitudeGain(ind));
                phase_vector.push_back(ofTFphase(ind));
            }
        }
private:
        inline double ofTFMagnitude(int16_t freq)
        {
            double omega = (2*M_PI*freq)/ofAngularCutFreq();
            double first_denom = 1-qPow(omega, 2);
            double second_denom = (1/ofQualityFactor())*omega;
            return 1./(first_denom+second_denom);
        }

        inline double ofTFMagnitudeGain(int16_t freq)
        {
            return 20*std::log10(ofTFMagnitude(freq));
        }

        inline double ofTFphase(int16_t freq)
        {
            double omega = (2*M_PI*freq)/ofAngularCutFreq();
            double numerator = (1/ofQualityFactor())*omega;
            double denominator = 1-qPow(omega, 2);
            return (-1)*qAtan(numerator/denominator);
        }

        int16_t m_freq=0;
        int16_t m_rload=0;
};

#endif // OUTFILTER_H
