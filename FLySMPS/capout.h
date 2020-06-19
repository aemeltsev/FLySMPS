#ifndef CAPOUT_H
#define CAPOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class CapOut
{
public:
    CapOut(double vr, double esr, double cpo, double vo, double crfq): volts_rippl(vr), esr_perc(esr), curr_peak_out(cpo), volts_out(vo), cros_frq_start_val(crfq){}
    double ocESRCapOut();
    double ocCapOutValue(const InputValue& ivalue);
    double ocCurrOurRMS(const FBTransformer& fbtvalue);
    double ocCapOutLoss(const FBTransformer &fbtvalue);

private:
    double volts_rippl;
    double esr_perc;
    double curr_peak_out;
    double volts_out;
    double cros_frq_start_val;//good starting value for crossover frequency(1/20 to 1/10)
    double ocTimeCapCharg(const InputValue& ivalue);
    double ocCurrCap();
};

#endif // CAPOUT_H
