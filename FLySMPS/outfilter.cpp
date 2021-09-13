#include "outfilter.h"

OutFilter::OutFilter(int32_t fr, int32_t rload, QObject *parent) noexcept
    :QObject(parent)
    ,m_freq(fr)
    ,m_rload(rload)
{}

inline double OutFilter::ofOutRipplVolt() const
{
    return (1./(ofAngularCutFreq()*ofCapacitor()))/((1./(ofAngularCutFreq()*ofCapacitor()))+(ofAngularCutFreq()*ofInductor()));
}

void  OutFilter::ofPlotArray(QVector<double> &freq_vector, QVector<double> &mag_vector, QVector<double> &phase_vector, int32_t begin, int32_t end, int32_t step)
{
    for(int32_t ind=begin; ind<end; ind+=step)
    {
        freq_vector.push_back(ind);
        mag_vector.push_back(ofTFMagnitudeGain(ind));
        phase_vector.push_back(ofTFPhaseAng(ind));
    }

    emit arrayComplete();
}

inline double OutFilter::ofTFMagnitude(const double freq)
{
    double omega = (2*M_PI*freq);
    return (1/(ofInductor()*ofCapacitor())) / (1/(ofInductor()*ofCapacitor()) + omega*(m_rload/ofInductor()) + qPow(omega, 2));
}

inline double OutFilter::ofTFMagnitudeGain(const double freq)
{
    return 20*std::log10(ofTFMagnitude(freq));
}

inline double OutFilter::ofTFPhaseAng(const double freq)
{
    return /*(-1) * */ofTFphase(freq);
}

inline double OutFilter::ofTFphase(const double freq)
{
    double omega = (2*M_PI*freq);
    return qAtan(((ofInductor() * omega) / m_rload) - (1. / ofInductor() * omega * ofCapacitor())) * (M_PI_DEG/M_PI);
}
