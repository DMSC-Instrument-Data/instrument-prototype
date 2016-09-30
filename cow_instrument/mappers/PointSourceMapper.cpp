#include "PointSourceMapper.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(PointSourceMapper);

bool PointSourceMapper::visit(const PointSample *const) { return false; }

bool PointSourceMapper::visit(const PointSource *const item) {
  this->store(*item);
  return true;
}

template <class Archive>
void PointSourceMapper::serialize(Archive &ar, const unsigned int version) {
  using namespace boost::serialization;
  ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ComponentVisitor);
  base_serialization(ar, version);
}

template void PointSourceMapper::serialize(boost::archive::text_oarchive &ar,
                                           const unsigned int version);
template void PointSourceMapper::serialize(boost::archive::text_iarchive &ar,
                                           const unsigned int version);
