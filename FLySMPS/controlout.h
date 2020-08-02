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
    /**
     * @brief PCSSM
     * @param tr
     * @param lp
     * @param lres
     * @param vin
     * @param vout
     * @param cout
     * @param esr
     */
    PCSSM(float tr, double lp,
          float lres, int16_t vin,
          int16_t vout, double cout,
          double esr):
        turnrat(tr), priminduct(lp),
        resload(lres), voltin(vin),
        voltout(vout), capout(cout),
        esrcap(esr)
    {}
    /**
     * @brief coZeroTwoAngFreq
     * @return
     */
    inline double coZeroTwoAngFreq() const
    {
        double tmp = std::pow(turnrat, 2)*static_cast<double>(resload);
        return tmp/(priminduct*voltrat*(voltrat+1));
    }
    /**
     * @brief coZeroOneAngFreq
     * @return
     */
    inline double coZeroOneAngFreq() const
    {
        return 1/(capout*esrcap);
    }
    /**
     * @brief coPoleTwoAngFreq
     * @return
     */
    inline double coPoleTwoAngFreq() const
    {
        return (std::pow(turnrat,2)*static_cast<double>(resload))/(priminduct*std::pow((voltrat+1),2));
    }
    /**
     * @brief coPoleOneAngFreq
     * @return
     */
    inline double coPoleOneAngFreq() const
    {
        return 2/(static_cast<double>(resload)*capout);
    }
    /**
     * @brief coGainZero
     * @param duty
     * @param fsw
     * @param vs
     * @return
     */
    inline double coGainZero(double duty, float fsw, float vs = 2.0f) const
    {
        double tmp = duty/static_cast<double>(vs);
        double load = std::sqrt(static_cast<double>(resload)/(2.*priminduct*static_cast<double>(fsw)));
        return tmp*voltin*load;
    }
    /**
     * @brief coGetExternAddVolt
     * @param se
     */
    void coGetExternAddVolt(double se){sawvolt = se;}
    /**
     * @brief coCurrDetectSlopeVolt
     * @param rsense
     * @return
     */
    inline double coCurrDetectSlopeVolt(double rsense) const
    {
        return (voltin*rsense)/priminduct;
    }
    /**
     * @brief coTimeConst
     * @param fsw
     * @return
     */
    inline double coTimeConst(float fsw) const
    {
        return (2*priminduct*static_cast<double>(fsw))/(std::pow(turnrat,2)*static_cast<double>(resload));
    }
    /**
     * @brief coGainCurrModeContrModulator
     * @param rsense
     * @param fsw
     * @return
     */
    inline double coGainCurrModeContrModulator(double rsense, float fsw) const
    {
        return 1/((coCurrDetectSlopeVolt(rsense)+sawvolt)*coTimeConst(fsw));
    }
private:
    float turnrat;
    double priminduct;
    float resload;
    int16_t voltin;
    int16_t voltout;
    double capout;
    double esrcap;
    inline void coVoltRatio() {voltrat = voltout/voltin;}
    double voltrat;
    double sawvolt;

};

class FCCD
{
public:
    FCCD();
};

#endif // CONTROLOUT_H
