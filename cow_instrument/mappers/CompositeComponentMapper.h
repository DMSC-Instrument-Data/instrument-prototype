#ifndef COMPOSITE_COMPONENT_MAPPER_H
#define COMPOSITE_COMPONENT_MAPPER_H

#include <boost/serialization/serialization.hpp>
#include "CompositeComponent.h"
#include "ComponentIdTypeMapper.h"
#include "SingleItemMapper.h"
#include "VectorOfComponentMapper.h"
#include <string>

class CompositeComponentMapper {

public:
  CompositeComponentMapper(const CompositeComponent &source);
  CompositeComponentMapper() = default;

  ComponentIdTypeMapper componentIdMapper;
  VectorOfComponentMapper posMapper;
  SingleItemMapper<std::string> nameMapper;

  CompositeComponent create();

  void store(const CompositeComponent &source);

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    using namespace boost::serialization;
    boost::serialization::serialize(ar, componentIdMapper, version);
    // boost::serialization::serialize(ar, posMapper, version);
  }
};

#endif
