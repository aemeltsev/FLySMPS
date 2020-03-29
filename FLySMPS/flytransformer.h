#ifndef FLYTRANSFORMER_H
#define FLYTRANSFORMER_H


class FlyTransformer
{
public:
    FlyTransformer();

    double MaxDutyCycle();
    double InpPower();

    double VDSOn();
    double VDSMax();

    double IPAver();
    double IPPeakPeak();
    double IPMax();
    double IPValley();
    double IPRMS();

    double PInduct();


};

#endif // FLYTRANSFORMER_H
