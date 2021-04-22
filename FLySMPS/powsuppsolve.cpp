#include "powsuppsolve.h"

PowSuppSolve::PowSuppSolve(QObject *parent)
{
    m_db.reset(new DBridge);
    m_bc.reset(new BCap);
    m_pm.reset(new PMosfet);
    m_ptpe.reset(new PulseTransPrimaryElectr());
    m_ptsw.reset(new PulseTransSecondWired());

    m_od = new ODiode();
    m_fod = new FullOutDiode();
    m_oc = new OCap();
    m_foc = new FullOutCap();
    m_of = new FullOutFilter();
    m_pssm = new PowerStageSmallSignalModel();
    m_ofs = new OptocouplerFedbackStage();


}

PowSuppSolve::~PowSuppSolve()
{
    delete m_od;
    delete m_fod;
    delete m_oc;
    delete m_foc;
    delete m_of;
    delete m_pssm;
    delete m_ofs;
}

