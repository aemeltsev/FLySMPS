#ifndef OUTFILTER_H
#define OUTFILTER_H
#include <QtMath>
#include <QVector>
#include <cmath>
#include <cstdint>

#define M_J        -1
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
        void ofPlotArray(QVector<double> &magnitudevector, QVector<double> &phasevector, int16_t begin, int16_t end, int16_t step)
        {
            for(int16_t ind=begin; ind<end; ind+=step)
            {
                magnitudevector.push_back(ofTFMagnitudeGain(ind));
                phasevector.push_back(ofTFphase(ind));
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
        /*        inline double ofTransferFunc(int16_t m_freq)
                {
                    return 1./qSqrt(1 + qPow(((2*M_PI*m_freq)/ofAngularCutFreq()), 4));
                }
                inline double ofTransFuncGainDB(int16_t frq)
                {
                    return 20*std::log10(ofTransferFunc(frq));
                }
                inline double ofTransfFuncPhase(int16_t frq)
                {
                    return M_J*(qAtan((2*M_PI*frq)/ofAngularCutFreq())*(M_PI_DEG/M_PI));
                }
        */

        int16_t m_freq=0;
        int16_t m_rload=0;
};

#endif // OUTFILTER_H
