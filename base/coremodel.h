#ifndef COREMODEL_H
#define COREMODEL_H

#include <QObject>
#include <QMap>

namespace db {

enum class CoreType
{
    TOR = 0x01,
    UU  = 0x02,
    EE  = 0x03,
    ETD = 0x04,
    RM  = 0x05,
    PQ  = 0x06,
    PM  = 0x07,
    EP  = 0x08,
    EPX = 0x09,
    EPO = 0x0A,
    P   = 0x0B,
    ELP = 0x0C,
    EQ  = 0x0D,
    ER  = 0x0E,
    EFD = 0x0F,
    EV  = 0x10,
    UI  = 0x11,
    UNDEF = 0x00
};

const QString unknown = "UNKNOWN";
const QMap<CoreType, QString> type2name = {
    {CoreType::UNDEF, "UNDEF"},
    {CoreType::TOR, "TOR"},
    {CoreType::UU, "UU"},
    {CoreType::EE, "EE"},
    {CoreType::ETD, "ETD"},
    {CoreType::RM, "RM"},
    {CoreType::PQ, "PQ"},
    {CoreType::PM, "PM"},
    {CoreType::EP, "EP"},
    {CoreType::EPX, "EPX"},
    {CoreType::EPO, "EPO"},
    {CoreType::P, "P"},
    {CoreType::ELP, "ELP"},
    {CoreType::EQ, "EQ"},
    {CoreType::ER, "ER"},
    {CoreType::EFD, "EFD"},
    {CoreType::EV, "EV"},
    {CoreType::UI, "UI"}
};

const QMap<QString, CoreType> name2type = {
    {"UNDEF", CoreType::UNDEF},
    {"TOR", CoreType::TOR},
    {"UU", CoreType::UU},
    {"EE", CoreType::EE},
    {"ETD", CoreType::ETD},
    {"RM", CoreType::RM},
    {"PQ", CoreType::PQ},
    {"PM", CoreType::PM},
    {"EP", CoreType::EP},
    {"EPX", CoreType::EPX},
    {"EPO", CoreType::EPO},
    {"P", CoreType::P},
    {"ELP", CoreType::ELP},
    {"EQ", CoreType::EQ},
    {"ER", CoreType::ER},
    {"EFD", CoreType::EFD},
    {"EV", CoreType::EV},
    {"UI", CoreType::UI}
};

//see Kazimierczuk M.-High-frequency magnetinc components - 2.11 Core Geometries
struct Geometry
{
    QString model_;
    CoreType type_ = CoreType::UNDEF;
    //GeometryToroidal - H, innerDiam, outerDiam
    //GeometryUU - D, F, E, G
    //GeometryEE - C, B, F, A, E, D
    //GeometryETD - C, B, F, A, E, D
    double H;
    double innerDiam;
    double outerDiam;
    double C;
    double B;
    double F;
    double A;
    double E;
    double D;
    double G;

    Geometry(){}
    Geometry(const QString& model, CoreType type, double h, double id, double od, double c, double b, double f, double a, double e, double d, double g)
        :model_(model)
        ,type_(type)
        ,H(h)
        ,innerDiam(id)
        ,outerDiam(od)
        ,C(c)
        ,B(b)
        ,F(f)
        ,A(a)
        ,E(e)
        ,D(d)
        ,G(g)
    {}
};

/* Properties of the magnetic material and using in
     * medium and high frequency switch-mode power supplies.
     */
struct Material
{
    QString materialName;
    int highRelativePermeability; //mu_rc
    int coerciveField; //H_c
    int tempCurie; //T_c
    int coreLossesRelative; //P_v
    int upperOperatingFrequency; //f_H
    double fluxDensity; //B_s
    double electricalResistivity; //rho_c

    Material()
        :materialName("")
        ,highRelativePermeability(0)
        ,coerciveField(0)
        ,tempCurie(0)
        ,coreLossesRelative(0)
        ,upperOperatingFrequency(0)
        ,fluxDensity(0.)
        ,electricalResistivity(0.)
    {}

    Material(const QString& name, int mu_rc, int h_c, int t_c, int p_v, int f_h, double b_s, double rho_c)
        :materialName(name)
        ,highRelativePermeability(mu_rc)
        ,coerciveField(h_c)
        ,tempCurie(t_c)
        ,coreLossesRelative(p_v)
        ,upperOperatingFrequency(f_h)
        ,fluxDensity(b_s)
        ,electricalResistivity(rho_c)
    {}

};

/* Properties of the gapped core. Using for saturation prevention,
     *  and work in the required temperature window.
     */
struct Gapping
{
    QString modelName;
    int actualRelativePermeability; //mu_e
    double inductanceFactor; //A_L
    double gapLength; //g
    double actualCoreLosses; //P_v

    Gapping()
        :modelName("")
        ,actualRelativePermeability(0)
        ,inductanceFactor(0.)
        ,gapLength(0.)
        ,actualCoreLosses(0.)
    {}

    Gapping(const QString& name, int mu_e, double a_l, double g, double p_v)
        :modelName(name)
        ,actualRelativePermeability(mu_e)
        ,inductanceFactor(a_l)
        ,gapLength(g)
        ,actualCoreLosses(p_v)
    {}
};

CoreType getCoreType(const QString &type);
const QString& getCoreString(CoreType type);

class CoreModel
{
public:
    CoreModel();
    CoreModel(CoreType type, bool gapped);
    CoreModel(const CoreModel& other);
    CoreModel& operator=(const CoreModel& other);
    ~CoreModel();

    void id(int deviceId){deviceId_ = deviceId;}
    void name(const QString& name){name_ = name;}
    void model(const QString& model){model_ = model;}
    void gapped(bool gapped){gapped_ = gapped;}
    void type(CoreType type){coreType_ = type;}
    void type(const QString& type);
    void coreMaterial(Material material);
    void coreGapping(Gapping gapping);
    void resistanceFactor(double resistanceFactor){resistanceFactor_ = resistanceFactor;}
    void effectiveMagneticVolume(int effectiveMagneticVolume){effectiveMagneticVolume_ = effectiveMagneticVolume;}
    void windowCrossSection(double windowCrossSection){windowCrossSection_ = windowCrossSection;}
    void effectiveMagneticPathLength(double effectiveMagneticPathLength){effectiveMagneticPathLength_ = effectiveMagneticPathLength;}
    void effectiveMagneticCrossSection(double effectiveMagneticCrossSection){effectiveMagneticCrossSection_ = effectiveMagneticCrossSection;}
    void lengthTurn(double lengthTurn){lengthTurn_ = lengthTurn;}
    void geometry(const Geometry& geom);

    const int& id() const {return deviceId_;}
    const QString& name() const {return name_;}
    const QString& model() const {return model_;}
    const bool& gapped() const {return gapped_;}
    const CoreType& type() const {return coreType_;}
    const Material& coreMaterial() const {return coreMaterial_;}
    const Gapping& coreGapping() const {return coreGapping_;}
    double resistanceFactor() const {return resistanceFactor_;}
    int effectiveMagneticVolume() const {return effectiveMagneticVolume_;}
    double windowCrossSection() const {return windowCrossSection_;}
    double effectiveMagneticPathLength() const {return effectiveMagneticPathLength_;}
    double effectiveMagneticCrossSection() const {return effectiveMagneticCrossSection_;}
    double lengthTurn() const {return lengthTurn_;}
    const Geometry& geometry() const {return geometry_;}

private:
    int deviceId_;
    QString name_;
    QString model_;
    bool gapped_;
    CoreType coreType_;
    Material coreMaterial_;
    Gapping coreGapping_;
    double resistanceFactor_; //P_v
    int effectiveMagneticVolume_; //V_e
    double windowCrossSection_; //A_N
    double effectiveMagneticPathLength_; //l_e
    double effectiveMagneticCrossSection_; //A_e
    double lengthTurn_; //l_N
    Geometry geometry_;
};

} // namespace db

#endif // COREMODEL_H
