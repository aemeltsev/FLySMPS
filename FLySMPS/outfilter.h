#ifndef OUTFILTER_H
#define OUTFILTER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class OutFilter
{
public:
    OutFilter(int16_t fr, double cap,
              double qf = 0.707, double dr = 1.):
        freq(fr), capacity(cap),
        qualfact(qf), damprat(dr)
    {}

    /**
      * @brief
      */
    inline double ofAngularCutFreq() const {return 2*S_PI*freq;}
    /**
      * @brief
      */
    inline double ofInductance() const {return  1/(pow((ofAngularCutFreq()),2) * capacity);}
    /**
      * @brief
      */
    inline double ofLoadResistance() const {return 1/(sqrt(2) * capacity * ofAngularCutFreq());}
    /******************************************************************/
    /**
      * @brief
      */
    inline double ofQualityFactor(){return ofLoadResistance() * sqrt(capacity/ofInductance());}
    /**
      * @brief
      */
    inline double ofDampingRatio(){return ofInductance()/(2 * ofQualityFactor());}
    /**
      * @brief
      */
    inline double ofCuttOffFreq(){return 1./sqrt(capacity * ofInductance());}
    /******************************************************************/
    /**
      * @brief
      */
    inline double ofAngularFreq(int16_t frq) const {return 2*S_PI*frq;}
    /**
      * @brief inductor impedances using Euler's formula
      */
    inline double ofIndImpedance(int16_t frq){return ofAngularFreq(frq) * ofInductance();}
    /**
      * @brief
      */
    inline double ofCapImpedance(int16_t frq){return 1/(ofAngularFreq(frq) * capacity);}
    /**
      * @brief
      */
    inline double ofCRParallResist(int16_t frq)
    {
        return (ofCapImpedance(frq)*ofLoadResistance())/(ofCapImpedance(frq)+ofLoadResistance());
    }
    /**
      * @brief
      */
    inline double ofFilterGainDB(int16_t frq)
    {
        return 20*std::log(std::abs(ofCRParallResist(frq)/(ofCRParallResist(frq)+ofIndImpedance(frq))));
    }
    /**
      * @brief
      */
    inline double ofOutRipplVolt()
    {
        return (1./(ofAngularCutFreq()*capacity))/((1./(ofAngularCutFreq()*capacity))+(ofAngularCutFreq()*ofInductance()));
    }
private:
    int16_t freq;
    double capacity;
    double qualfact;
    double damprat;
};

#endif // OUTFILTER_H
