#ifndef _SINGLEITEMMAPPER_H
#define _SINGLEITEMMAPPER_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/optional.hpp>
#include <stdexcept>

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

  bool isInitialized() const { return heldValue.is_initialized(); }

  Mapee create() const {
    if (isInitialized()) {
      return heldValue.get();
    } else {
      throw std::invalid_argument("SingleItemMapper::create failed. Mapper not "
                                  "provided with a serialization item");
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
