/**
  * License text...
*/
#ifndef DIODEBRIDGE_H
#define DIODEBRIDGE_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class DiodeBridge
{
public:
    /**
     * @brief DiodeBridge -
     * @param invmax - Max input AC line voltage V
     * @param invmin - Min input AC line voltage V
     * @param ef - Efficiency ratio
     * @param pout - Total max power output W
     * @param fl - Line frequency default 50Hz
     */
    DiodeBridge(int16_t max_volt, int16_t min_volt,
                float eff, float pout,
                float fl = 50.0f):
        ac_inp_volt_max(max_volt), ac_inp_volt_min(min_volt),
        efficiency(eff), pow_max_out(pout),
        freq_line(fl)
    {}

    /**
     * @brief setBcapParam
     * @param bcpc - bulk capacitor peak current
     * @param ct - the capacitor total charging time
     */
    void setBcapParam(float bcpc, double ct)
    {
        bcappc = bcpc;
        chrgtm = ct;}
    inline double IDiodePeak() const;
    inline double DiodeCurrentSlope() const;
    inline double DiodeConductTime() const;
    inline double ILoadAVG() const;
    inline double IDiodeAVG() const;
    inline double IDiodeRMS() const;
    inline double IDiodeRMSTot() const;
    inline double MinPeakInVoltage() const;
    inline double MaxPeakInVoltage() const;

private:
    int16_t ac_inp_volt_max;
    int16_t ac_inp_volt_min;
    float efficiency;
    float pow_max_out;
    float freq_line;

    double cap_char_time; // the capacitor total charging time
    float cap_peak_curr; // bulk capacitor peak current

    double cur_max_load = static_cast<double>(pow_max_out)/static_cast<double>((efficiency*ac_inp_volt_min)); // load maximum current
    double cur_min_load = static_cast<double>(pow_max_out)/static_cast<double>((efficiency*ac_inp_volt_max)); // load minimum current

};

#endif // DIODEBRIDGE_H
