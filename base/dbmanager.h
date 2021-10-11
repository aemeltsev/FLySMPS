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

    struct Gapped
    {
        QString coreName;
        QString materialName;
        double inductanceFactor; //A_L
        uint32_t actualRelativePermeability; //mu_e
        double gapLength; //g
        double actualCoreLosses; //P_v
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

    struct CorePot
    {
        QString coreName;
        uint16_t H;
        uint16_t G;
        uint16_t B;
        uint16_t A;
        uint16_t C;
        uint16_t E;
        uint16_t F;
        uint16_t D;
    };

    struct CorePQ
    {
        QString coreName;
        uint16_t C;
        uint16_t J;
        uint16_t A;
        uint16_t G;
        uint16_t l;
        uint16_t twB;
        uint16_t twD;
        uint16_t E;
        uint16_t F;
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
};

#endif //DBMANAGER_H
