/**
  Copyright 2021 Anton Emeltsev

  This file is part of FSMPS - asymmetrical converter model estimate.

  FSMPS tools is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FSMPS tools is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef CAPOUT_H
#define CAPOUT_H
#include <QtMath>
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
