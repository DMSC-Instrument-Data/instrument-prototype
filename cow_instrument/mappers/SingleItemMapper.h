#ifndef _SINGLEITEMMAPPER_H
#define _SINGLEITEMMAPPER_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/optional.hpp>

/**
 Single simple Mapper templated class. Creates simple mappers
 around classes with trivial serializable components.
 */
template <typename Mapee> class SingleItemMapper {

public:
  SingleItemMapper(const Mapee &mapee) : heldValue(mapee) {}
  SingleItemMapper() {}

  virtual ~SingleItemMapper() {}

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
  boost::optional<Mapee> heldValue;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    // Use overloaded serialization for optional bools.
    boost::serialization::serialize<Archive, Mapee>(ar, heldValue, version);
  }
};

#endif
