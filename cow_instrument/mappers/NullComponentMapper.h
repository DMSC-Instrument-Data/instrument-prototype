#ifndef NULLCOMPONENTMAPPER_H
#define NULLCOMPONENTMAPPER_H

#include "ComponentVisitor.h"
#include "boost/serialization/export.hpp"
#include "SingleItemMapper.h"
#include <string>

class NullComponentMapper : public ComponentVisitor {
public:
  NullComponentMapper() = default;
  NullComponentMapper(NullComponent &);
  virtual bool visit(DetectorComponent const *const component) override;
  virtual bool visit(ParabolicGuide const *const component) override;
  virtual bool visit(PointSample const *const component) override;
  virtual bool visit(PointSource const *const component) override;
  virtual bool visit(CompositeComponent const *const component) override;
  virtual bool visit(NullComponent const *const component) override;
  virtual Component *create();
  virtual ~NullComponentMapper() {}

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};
BOOST_CLASS_EXPORT_KEY(NullComponentMapper)

#endif
