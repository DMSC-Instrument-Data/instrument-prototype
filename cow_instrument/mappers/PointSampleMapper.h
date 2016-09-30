#ifndef POINTSAMPLEMAPPER_H
#define POINTSAMPLEMAPPER_H

#include "PointPathComponentMapper.h"
#include "PointSample.h"
#include <boost/serialization/export.hpp>

class PointSampleMapper : public PointPathComponentMapper<PointSample> {
public:
  using PointPathComponentMapper<PointSample>::PointPathComponentMapper;
  virtual bool visit(PointSample const *const item);
  virtual bool visit(PointSource const *const);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};
BOOST_CLASS_EXPORT_KEY(PointSampleMapper);
#endif
