#include <boost/serialization/array.hpp>
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
