#ifndef _DETECTORCOMPONENTMAPPER_H
#define _DETECTORCOMPONENTMAPPER_H

#include <boost/serialization/serialization.hpp>
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
  virtual bool visit(ParabolicGuide const *const) override { /*Do nothing*/
    return false;
  }
  virtual bool visit(PointSample const *const) override { /*Do nothing*/
    return false;
  }
  virtual bool visit(PointSource const *const) override { /*Do nothing*/
    return false;
  }

  virtual DetectorComponent *create() override;

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    using namespace boost::serialization;
    ar &BOOST_SERIALIZATION_BASE_OBJECT_NVP(ComponentVisitor);
    boost::serialization::serialize(ar, componentIdMapper, version);
    boost::serialization::serialize(ar, detectorIdMapper, version);
    boost::serialization::serialize(ar, posMapper, version);
  }
};

#endif
