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
    DiodeBridge(int16_t invmax, int16_t invmin,
                float ef, float pout,
                float fl = 50.0f):
        acinvmax(invmax), acinvmin(invmin),
        eff(ef), pmaxout(pout),
        frline(fl)
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
    int16_t acinvmax;
    int16_t acinvmin;
    float eff;
    float pmaxout;
    float frline;

    double chrgtm; // the capacitor total charging time
    float bcappc; // bulk capacitor peak current

    double curmaxl = static_cast<double>(pmaxout)/static_cast<double>((eff*acinvmin)); // load maximum current
    double curminl = static_cast<double>(pmaxout)/static_cast<double>((eff*acinvmax)); // load minimum current

};

#endif // DIODEBRIDGE_H
