#ifndef _INTTOTYPEMAPPER_H
#define _INTTOTYPEMAPPER_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/optional.hpp>

template <typename T> class IntToTypeMapper {

public:
  IntToTypeMapper(const T &value) : heldValue(value.value) {}

  IntToTypeMapper() = default;

  boost::optional<typename T::StorageType> heldValue;

  T create() {
    if (heldValue.is_initialized()) {
      return T(heldValue.get());
    } else {
      throw std::runtime_error("Not initialized;");
    }
  }

  void store(const T &source) { heldValue = source.value; }

  bool initalized() const { return heldValue.is_initialized(); }

protected:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    // Use overloaded serialization for optional bools.
    boost::serialization::serialize<Archive, typename T::StorageType>(
        ar, heldValue, version);
  }

};

#endif
