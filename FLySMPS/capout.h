#ifndef CAPOUT_H
#define CAPOUT_H
#include <QtMath>
#include <QtAlgorithms>
#include <cstdint>

struct CapOutProp
{
    int16_t co_volts_out;//Secondary output voltage
    int16_t co_curr_peak_out;//Secondary current peak
    float co_volts_rippl;//Ripple voltage of the secondary side
    float co_esr_perc;//ESR percentage(0.1–1.5 ohm)
    float co_cros_frq_start_val;//Crossover frequency(1/20 to 1/10 from working frequency)
};

class CapOut
{
public:
    CapOut(CapOutProp& cop){qSwap(m_cop, cop);}
    inline double ocESRCapOut() const;
    inline double ocCapOutValue(int16_t freq_switch) const;
    inline double ocCurrOurRMS(float actual_max_duty_cycle) const;
    inline double ocZeroFreqCapOut(int16_t freq_switch) const;
    inline double ocOutRippleVolt(double curout, int16_t ncap) const;
    inline double ocCapOutLoss(float actual_max_duty_cycle) const;

private:
    CapOutProp m_cop;
    inline double ocTimeCapCharg(int16_t freq_switch) const;
    inline double ocCurrCap() const;
};

#endif // CAPOUT_H
