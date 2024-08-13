#ifndef COREMODEL_H
#define COREMODEL_H

#include <QObject>

namespace db {

//see Kazimierczuk M.-High-frequency magnetinc components - 2.11 Core Geometries
struct GeometryToroidal
{
    int H;
    int innerDiam;
    int outerDiam;
    GeometryToroidal(){}
    GeometryToroidal(int h, int id, int od)
        :H(h)
        ,innerDiam(id)
        ,outerDiam(od)
    {}
};

struct GeometryUU
{
    int D;
    int F;
    int E;
    int G;
    GeometryUU(){}
    GeometryUU(int d, int f, int e, int g)
        :D(d)
        ,F(f)
        ,E(e)
        ,G(g)
    {}
};

struct GeometryEE
{
    int C;
    int B;
    int F;
    int A;
    int E;
    int D;
    GeometryEE(){}
    GeometryEE(int c, int b, int f, int a, int e, int d)
        :C(c)
        ,B(b)
        ,F(f)
        ,A(a)
        ,E(e)
        ,D(d)
    {}
};

struct GeometryETD
{
    int C;
    int B;
    int F;
    int A;
    int E;
    int D;
    GeometryETD(){}
    GeometryETD(int c, int b, int f, int a, int e, int d)
        :C(c)
        ,B(b)
        ,F(f)
        ,A(a)
        ,E(e)
        ,D(d)
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
    int actualRelativePermeability; //mu_e
    double inductanceFactor; //A_L
    double gapLength; //g
    double actualCoreLosses; //P_v

    Gapping()
        :actualRelativePermeability(0)
        ,inductanceFactor(0.)
        ,gapLength(0.)
        ,actualCoreLosses(0.)
    {}

    Gapping(int mu_e, double a_l, double g, double p_v)
        :actualRelativePermeability(mu_e)
        ,inductanceFactor(a_l)
        ,gapLength(g)
        ,actualCoreLosses(p_v)
    {}
};

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

class CoreModel
{
public:
    CoreModel();
    CoreModel(CoreType type, bool gapped);
    CoreModel(const CoreModel& other);
    CoreModel& operator=(const CoreModel& other);

    void id(int deviceId){deviceId_ = deviceId;}
    void name(const QString& name){name_ = name;}
    void model(const QString& model){model_ = model;}
    void gapped(bool gapped){gapped_ = gapped;}
    void type(CoreType type){coreType_ = type;}
    void type(const QString& type);
    void coreMaterial(Material material);
    void coreGapping(Gapping gapping);
    void resistanceFactor(int resistanceFactor){resistanceFactor_ = resistanceFactor;}
    void effectiveMagneticVolume(int effectiveMagneticVolume){effectiveMagneticVolume_ = effectiveMagneticVolume;}
    void windowCrossSection(int windowCrossSection){windowCrossSection_ = windowCrossSection;}
    void effectiveMagneticPathLength(double effectiveMagneticPathLength){effectiveMagneticPathLength_ = effectiveMagneticPathLength;}
    void effectiveMagneticCrossSection(double effectiveMagneticCrossSection){effectiveMagneticCrossSection_ = effectiveMagneticCrossSection;}
    void lenghTurn(double lenghTurn){lenghTurn_ = lenghTurn;}
    void torGeometry(const GeometryToroidal);
    void uuGeometry(const GeometryUU);
    void eeGeometry(const GeometryEE);
    void etdGeometry(const GeometryETD);

    const int& id() const {return deviceId_;}
    const QString& name() const {return name_;}
    const QString& model() const {return model_;}
    const bool& gapped() const {return gapped_;}
    const CoreType& type() const {return coreType_;}
    const Material& coreMaterial() const {return coreMaterial_;}
    const Gapping& coreGapping() const {return coreGapping_;}
    int resistanceFactor() const {return resistanceFactor_;}
    int effectiveMagneticVolume() const {return effectiveMagneticVolume_;}
    int windowCrossSection() const {return windowCrossSection_;}
    double effectiveMagneticPathLength() const {return effectiveMagneticPathLength_;}
    double effectiveMagneticCrossSection() const {return effectiveMagneticCrossSection_;}
    double lenghTurn() const {return lenghTurn_;}
    const GeometryToroidal& torGeometry() const {return torGeometry_;}
    const GeometryUU& uuGeometry() const {return uuGeometry_;}
    const GeometryEE& eeGeometry() const {return eeGeometry_;}
    const GeometryETD& etdGeometry() const {return etdGeometry_;}

private:
    int deviceId_;
    QString name_;
    QString model_;
    bool gapped_;
    CoreType coreType_;
    Material coreMaterial_;
    Gapping coreGapping_;
    int resistanceFactor_; //P_v
    int effectiveMagneticVolume_; //V_e
    int windowCrossSection_; //A_N
    double effectiveMagneticPathLength_; //l_e
    double effectiveMagneticCrossSection_; //A_e
    double lenghTurn_; //l_N
    GeometryToroidal torGeometry_;
    GeometryUU uuGeometry_;
    GeometryEE eeGeometry_;
    GeometryETD etdGeometry_;
};

} // namespace db
#endif // COREMODEL_H
