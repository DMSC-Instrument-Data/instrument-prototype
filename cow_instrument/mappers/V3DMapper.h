#ifndef _V3DMapper_H
#define _V3DMapper_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/array.hpp>
#include <boost/optional.hpp>

#include "V3D.h"

/**
 Single simple Mapper templated class. Creates simple mappers
 around classes with trivial serializable components.
 */
template <typename Mapee> class VanillaSingleItemMapper {

public:
  VanillaSingleItemMapper(const Mapee &mapee) : heldValue(mapee) {}
  VanillaSingleItemMapper() {}

  virtual ~VanillaSingleItemMapper() {}

  bool initalized() const { return heldValue.is_initialized(); }

  Mapee create() const {
    if (heldValue.is_initialized()) {
      return heldValue.get();
    } else {
      throw std::runtime_error("Not implemented");
    }
  }

  void store(const Mapee &mapee) { heldValue = mapee; }

protected:
  boost::optional<V3D> heldValue;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    // Use overloaded serialization for optional bools.
    boost::serialization::serialize<Archive, Mapee>(ar, heldValue, version);
  }
};

namespace boost {
namespace serialization {
#ifdef BOOST_NO_CXX11_HDR_ARRAY
template <class Archive, class T, size_t N>
void serialize(Archive &ar, std::array<T, N> &a, const unsigned int) {
  ar &boost::serialization::make_array(a.data(), a.size());
}
#endif
} // namespace serialization
} // namespace boost

class V3DMapper : public VanillaSingleItemMapper<V3D> {

public:
  using VanillaSingleItemMapper<V3D>::VanillaSingleItemMapper;

  virtual ~V3DMapper() {}
};

#endif
