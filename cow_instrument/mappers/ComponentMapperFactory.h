#ifndef COMPONENTMAPPERFACTORY_H
#define COMPONENTMAPPERFACTORY_H

#include <vector>
#include <memory>

class ComponentVisitor;

class ComponentMapperFactory {

public:
  using MapperFamily = ComponentVisitor;

  static std::vector<std::shared_ptr<ComponentVisitor>> createMappers();
};

#endif
