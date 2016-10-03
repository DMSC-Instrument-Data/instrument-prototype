#ifndef COMPONENTMAPPERFACTORY_H
#define COMPONENTMAPPERFACTORY_H

#include "ComponentVisitor.h"
#include "DetectorComponentMapper.h"
#include "NullComponentMapper.h"
#include "PointSampleMapper.h"
#include "PointSourceMapper.h"
#include <vector>
#include <memory>

class ComponentMapperFactory {

public:
  using MapperFamily = ComponentVisitor;

  static std::vector<std::shared_ptr<ComponentVisitor>> createMappers() {

    return std::vector<std::shared_ptr<ComponentVisitor>>{
        std::make_shared<DetectorComponentMapper>(),
        std::make_shared<NullComponentMapper>(),
        std::make_shared<PointSampleMapper>(),
        std::make_shared<PointSourceMapper>()};
  }
};

#endif
