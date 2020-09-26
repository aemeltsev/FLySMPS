#ifndef OUTFILTER_H
#define OUTFILTER_H
#include <cmath>
#include <cstdint>
#include <map>
#include <structdata.h>

class OutFilter
{
public:
    /**
     * @brief OutFilter -
     * @param fr -
     * @param cap -
     */
    OutFilter(int16_t fr, double cap
              /*double qf = 0.707, double dr = 1.*/):
        freq(fr), capacity(cap)
        /*qualfact(qf), damprat(dr)*/
    {}
    /**
     * @brief ofAngularCutFreq - Filter angular cut frequency
     * @return
     */
    inline double ofAngularCutFreq() const {return 2*S_PI*freq;}
    /**
     * @brief ofInductance - Inductance in H
     * @return
     */
    inline double ofInductance() const {return  1/(pow((ofAngularCutFreq()),2) * capacity);}
    /**
     * @brief ofLoadResistance - Resistance in Ohms
     * @return
     */
    inline double ofLoadResistance() const {return 1/(sqrt(2) * capacity * ofAngularCutFreq());}
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
    inline double ofCutOffFreq(){return 1./sqrt(capacity * ofInductance());}
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
    void ofPlotArray(std::map<int16_t, double>* tabmap, int16_t begin, int16_t end, int16_t step)
    {
        for(int16_t ind=begin; ind<end; ind+=step)
        {
            tabmap->insert(std::pair<int16_t, double>(ind, ofFilterGainDB(ind)));
        }
    }
private:
    /**
     * @brief ofAngularFreq - Angular cut frequency
     * @param frq - frequency
     * @return
     */
    inline double ofAngularFreq(int16_t frq) const {return 2*S_PI*frq;}
    /**
     * @brief ofIndImpedance - Inductor impedances using Euler's formula
     * @param frq - frequency
     * @return
     */
    inline double ofIndImpedance(int16_t frq){return ofAngularFreq(frq) * ofInductance();}
    /**
     * @brief ofCapImpedance - Complex resistance or "impedance" Cap
     * @param frq - frequency
     * @return
     */
    inline double ofCapImpedance(int16_t frq){return 1/(ofAngularFreq(frq) * capacity);}
    /**
     * @brief ofCRParallResist - parallel resistance C and R
     * @param frq - frequency
     * @return
     */
    inline double ofCRParallResist(int16_t frq)
    {
        return (ofCapImpedance(frq)*ofLoadResistance())/(ofCapImpedance(frq)+ofLoadResistance());
    }
    /**
     * @brief ofFilterGainDB - Gain in decibels
     * @param frq - frequency
     * @return
     */
    inline double ofFilterGainDB(int16_t frq)
    {
        return 20*std::log(std::abs(ofCRParallResist(frq)/(ofCRParallResist(frq)+ofIndImpedance(frq))));
    }

    int16_t freq;
    double capacity;
    //double qualfact;
    //double damprat;
};

#endif // OUTFILTER_H
