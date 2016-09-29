#ifndef _POINTPATHCOMPONENTMAPPER_H
#define _POINTPATHCOMPONENTMAPPER_H

#include "ComponentIdTypeMapper.h"
#include "V3DMapper.h"
#include <stdexcept>

template <typename Mapee> class PointPathComponentMapper {
public:
  using MapeeType = Mapee;

  ComponentIdTypeMapper componentIdMapper;
  V3DMapper posMapper;

  PointPathComponentMapper(const Mapee &source) { store(source); }
  PointPathComponentMapper() = default;

  Mapee create() {
    if (componentIdMapper.initalized() && posMapper.initialized()) {
      return Mapee(posMapper.create(), componentIdMapper.create());
    } else {
      throw std::invalid_argument("Cannot be deserialized. Not all mandatory "
                                  "construction fields have been provided for "
                                  "PointPathComponentMapper");
    }
  }

  void store(const Mapee &source) {
    componentIdMapper.store(source.componentId());
    posMapper.store(source.getPos());
  }

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    using namespace boost::serialization;
    boost::serialization::serialize(ar, componentIdMapper, version);
    boost::serialization::serialize(ar, posMapper, version);
  }
};

#endif
