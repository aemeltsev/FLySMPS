#include "powsuppsolve.h"

PowSuppSolve::PowSuppSolve(QObject *parent)
{
    m_db = new DBridge();
    m_bc = new BCap();
    m_pm = new PMosfet();
    m_od = new ODiode();
    m_fod = new FullOutDiode();
    m_oc = new OCap();
    m_foc = new FullOutCap();
    m_of = new FullOutFilter();
    m_pssm = new PowerStageSmallSignalModel();
    m_ofs = new OptocouplerFedbackStage();
    m_ptpe = new PulseTransPrimaryElectr();
    m_ptsw = new PulseTransSecondWired();
}

PowSuppSolve::~PowSuppSolve()
{
    delete m_db;
    delete m_bc;
    delete m_pm;
    delete m_od;
    delete m_fod;
    delete m_oc;
    delete m_foc;
    delete m_of;
    delete m_pssm;
    delete m_ofs;
    delete m_ptpe;
    delete m_ptsw;
}
