#ifndef _INTTOTYPEMAPPER_H
#define _INTTOTYPEMAPPER_H

#include "IdType.h"
//#include <boost/serialization/serialization.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/optional.hpp>

template <typename T> class IntToTypeMapper {

public:
  IntToTypeMapper(const T &value) : heldValue(value), m_initialized(true){};
  IntToTypeMapper() : heldValue(0), m_initialized(false){};

  T heldValue;

  T create() { return heldValue; }

  void store(const T &source) {
    heldValue = source;
    m_initialized = true;
  }

  bool initalized() const { return m_initialized; }

  template <class Archive> void load(Archive &ar, const unsigned int version) {
    ar &heldValue.value;
    m_initialized = true;
  }

  template <class Archive>
  void save(Archive &ar, const unsigned int version) const {
    ar &heldValue.value;
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
  bool m_initialized;
};

using ComponentIdTypeMapper = IntToTypeMapper<ComponentIdType>;
using DetectorIdTypeMapper = IntToTypeMapper<DetectorIdType>;

#endif
