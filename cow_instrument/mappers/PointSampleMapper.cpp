#include "PointSampleMapper.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(PointSampleMapper);

bool PointSampleMapper::visit(const PointSample *const item) {
  this->store(*item);
  return true;
}

bool PointSampleMapper::visit(const PointSource *const) { return false; }

template <class Archive>
void PointSampleMapper::serialize(Archive &ar, const unsigned int version) {
  using namespace boost::serialization;
  ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ComponentVisitor);
  base_serialization(ar, version);
}

template void PointSampleMapper::serialize(boost::archive::text_oarchive &ar,
                                           const unsigned int version);
template void PointSampleMapper::serialize(boost::archive::text_iarchive &ar,
                                           const unsigned int version);
