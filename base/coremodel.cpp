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
{}


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
}
