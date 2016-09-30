#ifndef _POINTPATHCOMPONENTMAPPER_H
#define _POINTPATHCOMPONENTMAPPER_H

#include "ComponentIdTypeMapper.h"
#include "ComponentVisitor.h"
#include "V3DMapper.h"
#include <stdexcept>
#include <boost/serialization/serialization.hpp>

template <typename Mapee>
class PointPathComponentMapper : public ComponentVisitor {
public:
  using MapeeType = Mapee;

  ComponentIdTypeMapper componentIdMapper;
  V3DMapper posMapper;

  PointPathComponentMapper(const Mapee &source) { store(source); }
  PointPathComponentMapper() = default;

  Mapee *create() {
    if (componentIdMapper.initalized() && posMapper.initialized()) {
      return new Mapee(posMapper.create(), componentIdMapper.create());
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

  virtual bool visit(DetectorComponent const *const) override { return false; }
  virtual bool visit(ParabolicGuide const *const) override { return false; }
  virtual bool visit(CompositeComponent const *const) override { return false; }
  virtual bool visit(NullComponent const *const) override { return false; }

protected:
  template <class Archive>
  void base_serialization(Archive &ar, const unsigned int version) {
    boost::serialization::serialize(ar, componentIdMapper, version);
    boost::serialization::serialize(ar, posMapper, version);
  }
};

#endif
