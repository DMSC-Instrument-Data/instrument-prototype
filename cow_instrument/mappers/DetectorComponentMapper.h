#ifndef _DETECTORCOMPONENTMAPPER_H
#define _DETECTORCOMPONENTMAPPER_H

#include <boost/serialization/serialization.hpp>
#include <boost/optional.hpp>

#include "IdType.h"
#include "DetectorComponent.h"
#include "V3D.h"

// TODO change header
#include "IntToTypeMapper.h"
#include "boost/serialization/optional.hpp"

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
class DetectorComponentMapper {
public:
  ComponentIdTypeMapper componentIdMapper;
  DetectorIdTypeMapper detectorIdMapper;
  boost::optional<V3D> pos;

  DetectorComponent create();
  void store(const DetectorComponent &source);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    componentIdMapper.serialize(ar, version);
    detectorIdMapper.serialize(ar, version);
    boost::serialization::serialize(ar, pos, version);
  }
};

#endif
