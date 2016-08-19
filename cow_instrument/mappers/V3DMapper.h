#ifndef _V3DMapper_H
#define _V3DMapper_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include "SingleItemMapper.h"
#include <Eigen/Core>
#include <array>

namespace boost {
namespace serialization {
#ifdef CUSTOM_STD_ARRAY_SERIALIZATION
template <class Archive, class T, size_t N>
void serialize(Archive &ar, std::array<T, N> &a, const unsigned int) {
  ar &boost::serialization::make_array(a.data(), a.size());
}
#endif
} // namespace serialization
} // namespace boost

class V3DMapper : public SingleItemMapper<std::array<double, 3> > {

public:

    V3DMapper(const Eigen::Vector3d &mapee) : SingleItemMapper(std::array<double, 3>{mapee[0],mapee[1],mapee[2]}) {}
    V3DMapper() = default;

    Eigen::Vector3d create() const {
      if (heldValue.is_initialized()) {
          auto arry = heldValue.value();
          return Eigen::Vector3d(arry[0], arry[1], arry[2]);
      } else {
        throw std::invalid_argument("V3DMapper::create failed. Mapper not "
                                    "provided with a serialization item");
      }
    }

    void store(const Eigen::Vector3d &mapee) { heldValue = std::array<double, 3>{mapee[0],mapee[1],mapee[2]}; }

  virtual ~V3DMapper() {}
};

#endif
