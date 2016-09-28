#ifndef _V3DMapper_H
#define _V3DMapper_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include "SingleItemMapper.h"
#include <Eigen/Core>
#include <array>

namespace boost {
namespace serialization {
#ifdef LEGACY_BOOST_SERIALIZATION
template <class Archive, class T, size_t N>
void serialize(Archive &ar, std::array<T, N> &a, const unsigned int) {
  ar &boost::serialization::make_array(a.data(), a.size());
}
#endif
} // namespace serialization
} // namespace boost

// Store an array internally but convert to a Eigen::Vector3d on demand
class V3DMapper : public SingleItemMapper<std::array<double, 3> > {

public:
  V3DMapper(const Eigen::Vector3d &mapee);
    V3DMapper() = default;

    Eigen::Vector3d create() const;

    void store(const Eigen::Vector3d &mapee);

    virtual ~V3DMapper();
};

#endif
