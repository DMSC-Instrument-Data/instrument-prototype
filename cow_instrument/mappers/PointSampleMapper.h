#ifndef POINTSAMPLEMAPPER_H
#define POINTSAMPLEMAPPER_H

#include "PointPathComponentMapper.h"
#include "PointSample.h"

class PointSampleMapper : public PointPathComponentMapper<PointSample> {
public:
  using PointPathComponentMapper<PointSample>::PointPathComponentMapper;
};

#endif
