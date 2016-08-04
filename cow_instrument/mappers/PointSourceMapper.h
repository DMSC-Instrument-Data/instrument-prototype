#ifndef POINTSOURCEMAPPER_H
#define POINTSOURCEMAPPER_H

#include "PointPathComponentMapper.h"
#include "PointSource.h"

class PointSourceMapper : public PointPathComponentMapper<PointSource> {
public:
  using PointPathComponentMapper<PointSource>::PointPathComponentMapper;
};

#endif
