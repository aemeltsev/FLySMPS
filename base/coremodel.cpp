#include <base/coremodel.h>

db::CoreModel::CoreModel()
    :deviceId_(-1)
    ,name_("")
    ,model_("")
    ,gapped_(false)
    ,coreType_(CoreType::UNDEF)
    ,coreMaterial_()
    ,coreGapping_()
    ,resistanceFactor_(0)
    ,effectiveMagneticVolume_(0)
    ,windowCrossSection_(0)
    ,effectiveMagneticPathLength_(0)
    ,effectiveMagneticCrossSection_(0)
    ,lenghTurn_(0)
    ,geometry_()
{}


db::CoreModel::CoreModel(CoreType type, bool gapped)
    :deviceId_(-1)
    ,name_("")
    ,model_("")
    ,gapped_(gapped)
    ,coreType_(type)
    ,coreMaterial_()
    ,coreGapping_()
    ,resistanceFactor_(0)
    ,effectiveMagneticVolume_(0)
    ,windowCrossSection_(0)
    ,effectiveMagneticPathLength_(0)
    ,effectiveMagneticCrossSection_(0)
    ,lenghTurn_(0)
    ,geometry_()
{
    geometry_.type_ = type;
}


db::CoreModel::CoreModel(const CoreModel& other)
    :deviceId_(other.deviceId_)
    ,name_(other.name_)
    ,model_(other.model_)
    ,gapped_(other.gapped_)
    ,coreType_(other.coreType_)
    ,resistanceFactor_(other.resistanceFactor_)
    ,effectiveMagneticVolume_(other.effectiveMagneticVolume_)
    ,windowCrossSection_(other.windowCrossSection_)
    ,effectiveMagneticPathLength_(other.effectiveMagneticPathLength_)
    ,effectiveMagneticCrossSection_(other.effectiveMagneticCrossSection_)
    ,lenghTurn_(other.lenghTurn_)
{
    coreMaterial(other.coreMaterial_);
    coreGapping(other.coreGapping_);
    geometry(other.geometry_);
}

void db::CoreModel::type(const QString &type)
{
    coreType_ = getCoreType(type);
}

void db::CoreModel::coreMaterial(db::Material material)
{
    coreMaterial_.materialName = material.materialName;
    coreMaterial_.highRelativePermeability = material.highRelativePermeability;
    coreMaterial_.coerciveField = material.coerciveField;
    coreMaterial_.tempCurie = material.tempCurie;
    coreMaterial_.coreLossesRelative = material.coreLossesRelative;
    coreMaterial_.upperOperatingFrequency = material.upperOperatingFrequency;
    coreMaterial_.fluxDensity = material.fluxDensity;
    coreMaterial_.electricalResistivity = material.electricalResistivity;
}

void db::CoreModel::coreGapping(db::Gapping gapping)
{
    if(gapped_ == true){
        coreGapping_.modelName = gapping.modelName;
        coreGapping_.actualRelativePermeability = gapping.actualRelativePermeability;
        coreGapping_.inductanceFactor = gapping.inductanceFactor;
        coreGapping_.gapLength = gapping.gapLength;
        coreGapping_.actualCoreLosses = gapping.actualCoreLosses;
    }
    else{return;}
}

void db::CoreModel::geometry(const db::Geometry& geom)
{
    geometry_.model_ = geom.model_;
    geometry_.type_ = geom.type_;
    geometry_.H = geom.H;
    geometry_.innerDiam = geom.innerDiam;
    geometry_.outerDiam = geom.outerDiam;
    geometry_.C = geom.C;
    geometry_.B = geom.B;
    geometry_.F = geom.F;
    geometry_.A = geom.A;
    geometry_.E = geom.E;
    geometry_.D = geom.D;
    geometry_.G = geom.G;
}
