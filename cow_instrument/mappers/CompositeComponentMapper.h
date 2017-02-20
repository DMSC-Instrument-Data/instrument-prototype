#ifndef COMPOSITE_COMPONENT_MAPPER_H
#define COMPOSITE_COMPONENT_MAPPER_H

#include <boost/serialization/serialization.hpp>
#include "CompositeComponent.h"
#include "ComponentIdTypeMapper.h"
#include "ComponentVisitor.h"
#include "SingleItemMapper.h"
#include "VectorOfComponentMapper.h"
#include <string>

class CompositeComponentMapper : public ComponentVisitor {

public:
  CompositeComponentMapper(const CompositeComponent &source);
  CompositeComponentMapper() = default;

  ComponentIdTypeMapper componentIdMapper;
  VectorOfComponentMapper itemMapper;
  SingleItemMapper<std::string> nameMapper;

  CompositeComponent *create() override;

  void store(const CompositeComponent &source);

  virtual bool visit(DetectorComponent const *const) override;
  virtual bool visit(ParabolicGuide const *const) override;
  virtual bool visit(PointSample const *const) override;
  virtual bool visit(PointSource const *const) override;
  virtual bool visit(CompositeComponent const *const component) override;
  virtual bool visit(NullComponent const * const component) override;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};

BOOST_CLASS_EXPORT_KEY(CompositeComponentMapper);

#endif
