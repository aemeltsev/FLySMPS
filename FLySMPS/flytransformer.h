/**
  * License text...
*/
#ifndef FLYTRANSFORMER_H
#define FLYTRANSFORMER_H
#include <cmath>
#include <cstdint>
#include <structdata.h>

class FlyTransformer
{
public:
    FlyTransformer();

    /**/
    double numOutPower(double &OutCurr, double &OutVolt);//Power of output n
    double numCoeffPower(InputValue &ivalue, double &OutPower);//Output power coefficient(Kl)
    double numSecondary(FBTransformer &fbtvalue, InputValue &ivalue, double &OutVolt);
    double numTurnsRatio(FBTransformer &fbtvalue, double &NumTurns);//Primary to secondary turns ratio
    /**/

};

#endif // FLYTRANSFORMER_H
