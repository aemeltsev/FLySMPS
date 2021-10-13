#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

class DBManager: public QObject
{
    Q_OBJECT

public:
    struct Core
    {
        QString coreName;
        double effectiveMagneticPathLength; //l_e
        double effectiveMagneticCrossSection; //A_e
        uint32_t effectiveMagneticVolume; //V_e
        QString materialName;
        uint32_t windowCrossSection; //A_N
        double lenghTurn; //l_N
        uint32_t resistanceFactor; //P_v
    };

    struct Material
    {
        QString materialName;
        uint32_t highRelativePermeability; //mu_rc
        double fluxDensity; //B_s
        uint32_t coerciveField; //H_c
        double electricalResistivity; //rho_c
        uint32_t tempCurie; //T_c
        uint32_t coreLossesRelative; //P_v
        uint32_t upperOperatingFrequency; //f_H
    };

    struct Gapping
    {
        QString coreName;
        QString materialName;
        double inductanceFactor; //A_L
        uint32_t actualRelativePermeability; //mu_e
        double gapLength; //g
        double actualCoreLosses; //P_v
    };

    enum class CoreType
    {
        TOR = 1,
        UU,
        EE,
        ETD
    };

    //see Kazimierczuk M.-High-frequency magnetinc components
    struct CoreToroidal
    {
        QString coreName;
        uint16_t H;
        uint16_t inDiam;
        uint16_t exDiam;
    };

    struct CoreUU
    {
        QString coreName;
        uint16_t D;
        uint16_t F;
        uint16_t E;
        uint16_t G;
    };

    struct CoreEE
    {
        QString coreName;
        uint16_t C;
        uint16_t B;
        uint16_t F;
        uint16_t A;
        uint16_t E;
        uint16_t D;
    };

    struct CoreETD
    {
        QString coreName;
        uint16_t C;
        uint16_t B;
        uint16_t F;
        uint16_t A;
        uint16_t E;
        uint16_t D;
    };

    explicit DBManager(QObject *parent = nullptr);
    bool initSQL(QString dbPath="");
    bool createTable();
    bool addCore(const Core& core_data);
    bool addMaterial(const Material& material_data);
    bool addGapping(const Gapping& gap_data, bool gapped=true);
    bool addToroidGeometry(const CoreToroidal& tg);
    bool addUUGeometry(const CoreToroidal& tg);
    bool addEEGeometry(const CoreToroidal& tg);
    bool addETDGeometry(const CoreETD& tg);
    bool removeCore(const QString& name);
    bool removeMaterial(const QString& name);
    bool removeGeometry(const QString& name, CoreType crtype);
    bool getCoreProperties(const QString& core_name,
                           const QString& material_name,
                           Core& out_core,
                           Material& out_material,
                           Gapping& out_gapp,
                           bool gapped=true);
    bool getCoreGeometry(const QString& core_name, QVector<uint16_t> out_gm);
};

#endif //DBMANAGER_H
