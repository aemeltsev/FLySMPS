#ifndef OUTFILTER_H
#define OUTFILTER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class OutFilter
{
public:
    OutFilter(int16_t fr, double cap, double qf = 0.707, double dr = 1.):
        freq(fr), capacity(cap), qualfact(qf), damprat(dr)
    {}

    /**
      * @brief
      */
    inline double ofAngularCutFreq() const {return 2*S_PI*freq;}
    /**
      * @brief
      */
    inline double ofInductance() const {return  1/pow((capacity * ofAngularCutFreq()),2);}
    /**
      * @brief
      */
    inline double ofCapESR() const {return 1/(sqrt(2) * capacity * ofAngularCutFreq());}

private:
    int16_t freq;
    double capacity;
    double qualfact;
    double damprat;
};

#endif // OUTFILTER_H
