#include "capout.h"

double CapOut::ocESRCapOut()
{
    return (volts_rippl*esr_perc)/curr_peak_out;
}

double CapOut::ocTimeCapCharg(const InputValue &ivalue)
{
    double cross_frq = cros_frq_start_val*ivalue.freq_switch;
    return (1/(4*cross_frq))+(1/ivalue.freq_switch);
}

double CapOut::ocCurrCap()
{
    return curr_peak_out/2;
}

double CapOut::ocCapOutValue(const InputValue& ivalue)
{
    return (ocCurrCap()*ocTimeCapCharg(ivalue))/(2*(volts_out*volts_rippl));
}

double CapOut::ocCurrOurRMS(const FBTransformer& fbtvalue)
{
    return curr_peak_out*sqrt((fbtvalue.actual_max_duty_cycle/3))-curr_peak_out;
}

double CapOut::ocCapOutLoss(const FBTransformer& fbtvalue)
{
    return pow(ocCurrOurRMS(fbtvalue), 2)*ocESRCapOut();
}
