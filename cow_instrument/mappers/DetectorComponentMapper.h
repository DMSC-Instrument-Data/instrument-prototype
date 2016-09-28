#ifndef _DETECTORCOMPONENTMAPPER_H
#define _DETECTORCOMPONENTMAPPER_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include "DetectorComponent.h"
#include "ComponentIdTypeMapper.h"
#include "DetectorIdTypeMapper.h"
#include "V3DMapper.h"
#include "ComponentVisitor.h"

/**
 * Abstraction for serialization/deserialization using
 * boost serialization routines. Keeps domain type
 * DetectorComponent clean of persistance code.
 *
 * Also, constructing types using non-default constructors (required for
 *DetectorComponent)
 * is convoluted in boost serialization. Those aspects go away when we use
 *mappers (Default constructable)
 */
class DetectorComponentMapper : public ComponentVisitor {
public:
  DetectorComponentMapper(const DetectorComponent &source);
  DetectorComponentMapper() = default;

  ComponentIdTypeMapper componentIdMapper;
  DetectorIdTypeMapper detectorIdMapper;
  V3DMapper posMapper;

  void store(const DetectorComponent &source);

  virtual bool visit(DetectorComponent const *const component) override;
  virtual bool visit(ParabolicGuide const *const) override;
  virtual bool visit(PointSample const *const) override;
  virtual bool visit(PointSource const *const) override;
  virtual bool visit(CompositeComponent const *const) override;
  virtual DetectorComponent *create() override;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};

BOOST_CLASS_EXPORT_KEY(DetectorComponentMapper);

#endif
