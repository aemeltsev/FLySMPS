#ifndef CAPOUT_H
#define CAPOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class CapOut
{
public:
    CapOut(double vr, double esr,
           double cpo, double vo,
           double crfq):
        volts_rippl(vr), esr_perc(esr),
        curr_peak_out(cpo), volts_out(vo),
        cros_frq_start_val(crfq)
    {}
    inline double ocESRCapOut();
    inline double ocCapOutValue(const InputValue& ivalue);
    inline double ocCurrOurRMS(const FBPT &fbptval);
    inline double ocZeroFreqCapOut(const InputValue &ivalue);
    inline double ocOutRippleVolt(double curout, int16_t ncap);
    inline double ocCapOutLoss(const FBPT &fbptval);

private:
    double volts_rippl;
    double esr_perc;
    double curr_peak_out;
    double volts_out;
    double cros_frq_start_val;//good starting value for crossover frequency(1/20 to 1/10)
    inline double ocTimeCapCharg(const InputValue& ivalue);
    inline double ocCurrCap();
};

#endif // CAPOUT_H
