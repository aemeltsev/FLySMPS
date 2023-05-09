#ifndef DEFINES_H
#define DEFINES_H

#include <QObject>

//see Kazimierczuk M.-High-frequency magnetinc components - 2.11 Core Geometries
struct GeometryToroidal
{
    QString coreName;
    uint16_t H;
    uint16_t innerDiam;
    uint16_t outerDiam;
};

struct GeometryUU
{
    QString coreName;
    uint16_t D;
    uint16_t F;
    uint16_t E;
    uint16_t G;
};

struct GeometryEE
{
    QString coreName;
    uint16_t C;
    uint16_t B;
    uint16_t F;
    uint16_t A;
    uint16_t E;
    uint16_t D;
};

struct GeometryETD
{
    QString coreName;
    uint16_t C;
    uint16_t B;
    uint16_t F;
    uint16_t A;
    uint16_t E;
    uint16_t D;
};

/* Properties of the magnetic material and using in
     * medium and high frequency switch-mode power supplies.
     */
struct Material
{
    QString materialName;
    uint32_t highRelativePermeability; //mu_rc
    uint32_t coerciveField; //H_c
    uint32_t tempCurie; //T_c
    uint32_t coreLossesRelative; //P_v
    uint32_t upperOperatingFrequency; //f_H
    double fluxDensity; //B_s
    double electricalResistivity; //rho_c
};

/* Properties of the gapped core. Using for saturation prevention,
     *  and work in the required temperature window.
     */
struct Gapping
{
    uint32_t actualRelativePermeability; //mu_e
    double inductanceFactor; //A_L
    double gapLength; //g
    double actualCoreLosses; //P_v
};

enum class CoreType
{
    TOR = 0x01,
    UU  = 0x02,
    EE  = 0x03,
    ETD = 0x04,
    UNDEF = 0x00
};

struct Core
{
    QString coreName;
    Material coreMaterial;
    Gapping coreGapping;
    uint32_t resistanceFactor; //P_v
    uint32_t effectiveMagneticVolume; //V_e
    uint32_t windowCrossSection; //A_N
    double effectiveMagneticPathLength; //l_e
    double effectiveMagneticCrossSection; //A_e
    double lenghTurn; //l_N
};

#endif // DEFINES_H
