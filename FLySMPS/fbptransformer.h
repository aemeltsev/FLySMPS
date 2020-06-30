#ifndef FBPTRANSFORMER_H
#define FBPTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>


class FBPTransformer
{
public:
    FBPTransformer();

    double DutyCycleDCM(const InputValue &ivalue, const BCap &bcvalue);
    double InputPower(const InputValue &ivalue);
    double PriInduct(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue);

    /*All current primary side*/
    double CurrPriAver(const BCap &bcvalue, const FBPT &fbptval);
    double CurrPriPeakToPeak(const BCap &bcvalue, const FBPT &fbptval, const InputValue &ivalue);
    double CurrPriMax(const FBPT &fbptval);
    double CurrPriValley(const FBPT &fbptval);
    double CurrPriRMS(const FBPT &fbptval);
    /*All current primary side*/
private:
    double KRF;
};

class FBPTWinding
{
public:
    FBPTWinding();

    struct FBPTW
    {

    };

};
#endif // FBPTRANSFORMER_H
