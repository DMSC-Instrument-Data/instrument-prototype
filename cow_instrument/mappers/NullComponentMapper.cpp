#include "NullComponentMapper.h"
#include "NullComponent.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(NullComponentMapper);

NullComponentMapper::NullComponentMapper(NullComponent &)
{
}

bool NullComponentMapper::visit(DetectorComponent const *const ) {return false;}
bool NullComponentMapper::visit(ParabolicGuide const *const ) {return false;}
bool NullComponentMapper::visit(PointSample const *const ) {return false;}
bool NullComponentMapper::visit(PointSource const *const ) {return false;}
bool NullComponentMapper::visit(CompositeComponent const *const ) {return false;}
bool NullComponentMapper::visit(NullComponent const * const){
    return true;}


Component *NullComponentMapper::create()
{
    return new NullComponent;
}

template <class Archive> void NullComponentMapper::serialize(Archive &ar, const unsigned int version) {
ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(ComponentVisitor);
}

template void
NullComponentMapper::serialize(boost::archive::text_oarchive &ar,
                                    const unsigned int version);
template void
NullComponentMapper::serialize(boost::archive::text_iarchive &ar,
                                    const unsigned int version);


