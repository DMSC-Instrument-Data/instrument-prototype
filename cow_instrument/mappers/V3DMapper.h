#ifndef _V3DMapper_H
#define _V3DMapper_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>
#include "SingleItemMapper.h"
#include "V3D.h"

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

class V3DMapper : public SingleItemMapper<V3D> {

public:
  using SingleItemMapper<V3D>::SingleItemMapper;

  virtual ~V3DMapper() {}
};

#endif
