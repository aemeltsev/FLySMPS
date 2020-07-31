#ifndef CONTROLOUT_H
#define CONTROLOUT_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

enum PS_MODE
{
    CCM_MODE,
    DCM_MODE
};

class PCSSM
{
public:
    PCSSM(float tr, double lp,
          float lres, int16_t vin,
          int16_t vout, double cout,
          double esr):
        turnrat(tr), priminduct(lp),
        resload(lres), voltin(vin),
        voltout(vout), capout(cout),
        esrcap(esr)
    {}
    inline void coVoltRatio() {voltrat = voltout/voltin;}
    inline double coZeroRHPAngFreq() const
    {
        double tmp = std::pow(turnrat, 2)*static_cast<double>(resload);
        return tmp/(priminduct*voltrat*(voltrat+1));
    }
private:
    float turnrat;
    double priminduct;
    float resload;
    int16_t voltin;
    int16_t voltout;
    double capout;
    double esrcap;
    double voltrat;

};

class FCCD
{
public:
    FCCD();
};

#endif // CONTROLOUT_H
