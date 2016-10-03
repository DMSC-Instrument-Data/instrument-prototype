#ifndef _PARABOLICGUIDEMAPPER_H
#define _PARABOLICGUIDEMAPPER_H

#include <boost/serialization/serialization.hpp>

#include "ComponentIdTypeMapper.h"
#include "ParabolicGuide.h"
#include "SingleItemMapper.h"
#include "V3DMapper.h"

class ParabolicGuideMapper {

public:
  ParabolicGuideMapper(const ParabolicGuide &source);
  ParabolicGuideMapper() = default;

  ComponentIdTypeMapper componentIdMapper;
  SingleItemMapper<double> aMapper;
  SingleItemMapper<double> hMapper;
  V3DMapper posMapper;

  ParabolicGuide create();

  void store(const ParabolicGuide &source);

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    using namespace boost::serialization;
    boost::serialization::serialize(ar, componentIdMapper, version);
    boost::serialization::serialize(ar, aMapper, version);
    boost::serialization::serialize(ar, hMapper, version);
    boost::serialization::serialize(ar, posMapper, version);
  }
};

#endif
