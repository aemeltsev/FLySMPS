#ifndef CAPOUT_H
#define CAPOUT_H
#include <QtMath>
#include <cstdint>

class CapOut
{
public:
    /**
     * @brief CapOut
     * @param vr
     * @param esr
     * @param cpo
     * @param vo
     * @param crfq
     */
    CapOut(double vr, double esr,
           double cpo, double vo,
           double crfq):
        volts_rippl(vr), esr_perc(esr),
        curr_peak_out(cpo), volts_out(vo),
        cros_frq_start_val(crfq)
    {}
    inline double ocESRCapOut() const;
    inline double ocCapOutValue(int16_t freq_switch) const;
    inline double ocCurrOurRMS(float actual_max_duty_cycle) const;
    inline double ocZeroFreqCapOut(int16_t freq_switch) const;
    inline double ocOutRippleVolt(double curout, int16_t ncap) const;
    inline double ocCapOutLoss(float actual_max_duty_cycle) const;

private:
    double volts_rippl;
    double esr_perc;
    double curr_peak_out;
    double volts_out;
    double cros_frq_start_val;//good starting value for crossover frequency(1/20 to 1/10)
    inline double ocTimeCapCharg(int16_t freq_switch) const;
    inline double ocCurrCap() const;
};

#endif // CAPOUT_H
