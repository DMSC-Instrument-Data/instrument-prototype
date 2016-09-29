#ifndef COMPONENTMAPPER_H
#define COMPONENTMAPPER_H

#include "PolymorphicSerializer.h"
#include "ComponentVisitor.h"
#include "CompositeComponentMapper.h"
#include "DetectorComponentMapper.h"
#include "NullComponentMapper.h"

#include <vector>
#include <memory>

class ComponentMapperFactoryAll {

public:
  using MapperFamily = ComponentVisitor;

  static std::vector<std::shared_ptr<ComponentVisitor>> createMappers() {

    auto mappers = std::vector<std::shared_ptr<ComponentVisitor>>();
    mappers.emplace_back(std::make_shared<DetectorComponentMapper>());
    mappers.emplace_back(std::make_shared<CompositeComponentMapper>());
    mappers.emplace_back(std::make_shared<NullComponentMapper>());
    return mappers;
  }
};

using ComponentMapper = PolymorphicSerializer<ComponentMapperFactoryAll>;

#endif
