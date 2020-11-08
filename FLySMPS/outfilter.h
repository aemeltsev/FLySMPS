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
    OutFilter(int16_t fr, double cap):
        freq(fr), capacity(cap)
    {}
    /**
     * @brief ofAngularCutFreq - Filter angular cut frequency
     * @return
     */
    inline double ofAngularCutFreq() const {return 2*M_PI*freq;}
    /**
     * @brief ofLoadResistance - Resistance in Ohms
     * @return
     */
    inline double ofLoadResistance() const {return 1./(M_SQRT2 * capacity * ofAngularCutFreq());}
    /**
     * @brief ofInductance - Inductance in H
     * @return
     */
    inline double ofInductance() const {return  (ofLoadResistance() * M_SQRT2)/(ofAngularCutFreq());}
    /******************************************************************/
    /**
     * @brief ofQualityFactor - Q factor
     * @return
     */
    inline double ofQualityFactor(){return ofLoadResistance() * sqrt(capacity/ofInductance());}
    /**
     * @brief ofDampingRatio - The damping ratio(\zeta)
     * @return
     */
    inline double ofDampingRatio(){return ofInductance()/(2 * ofQualityFactor());}
    /**
     * @brief ofCutOffFreq - Cutoff Frequency
     * @return
     */
    inline double ofCutOffFreq(){return 1./(2*M_PI*qSqrt(capacity * ofInductance()));}
    /**
     * @brief ofOutRipplVolt - Output ripple voltage
     * @return
     */
    inline double ofOutRipplVolt()
    {
        return (1./(ofAngularCutFreq()*capacity))/((1./(ofAngularCutFreq()*capacity))+(ofAngularCutFreq()*ofInductance()));
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
            magnitudevector.push_back(ofTransFuncGainDB(ind));
            phasevector.push_back(ofTransfFuncPhase(ind));
        }
    }
private:
    /**
     * @brief ofTransferFunc
     * @param frq
     * @return
     */
    inline double ofTransferFunc(int16_t frq)
    {
        return 1./qSqrt(1 + qPow(((2*M_PI*frq)/ofAngularCutFreq()), 4));
    }
    /**
     * @brief ofFilterGainDB - Gain in decibels
     * @param frq - frequency
     * @return
     */
    inline double ofTransFuncGainDB(int16_t frq)
    {
        return 20*std::log10(ofTransferFunc(frq));
    }
    /**
     * @brief ofTransfFuncPhase
     * @param frq
     * @return
     */
    inline double ofTransfFuncPhase(int16_t frq)
    {
        return M_J*(qAtan((2*M_PI*frq)/ofAngularCutFreq())*(M_PI_DEG/M_PI));
    }

    int16_t freq=0;
    double capacity=0;
};

#endif // OUTFILTER_H
