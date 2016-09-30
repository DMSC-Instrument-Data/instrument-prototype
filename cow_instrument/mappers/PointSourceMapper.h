#ifndef POINTSOURCEMAPPER_H
#define POINTSOURCEMAPPER_H

#include "PointPathComponentMapper.h"
#include "PointSource.h"
#include <boost/serialization/export.hpp>

class PointSourceMapper : public PointPathComponentMapper<PointSource> {
public:
  using PointPathComponentMapper<PointSource>::PointPathComponentMapper;
  virtual bool visit(PointSource const *const item) override;
  virtual bool visit(PointSample const *const) override;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};
BOOST_CLASS_EXPORT_KEY(PointSourceMapper);

#endif
