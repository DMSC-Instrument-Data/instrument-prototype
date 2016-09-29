#ifndef SHAREDPTRSERIALIZATION_H
#define SHAREDPTRSERIALIZATION_H

#include <boost/serialization/shared_ptr.hpp>
#include <memory>

namespace boost {
namespace serialization {
#ifdef LEGACY_BOOST_SERIALIZATION
template<class Archive, class T>
inline void save(
    Archive & ar,
    const std::shared_ptr< T > &t,
    const unsigned int /* file_version */
){
    const T * t_ptr = t.get();
    ar << boost::serialization::make_nvp("px", t_ptr);
}

template<class Archive, class T>
inline void load(
    Archive & ar,
    std::shared_ptr< T > &t,
    const unsigned int /*file_version*/
){

    T* r;
    ar >> boost::serialization::make_nvp("px", r);
    t.reset(r);
}

template<class Archive, class T>
inline void serialize(
    Archive & ar,
    std::shared_ptr< T > &t,
    const unsigned int file_version
){
    // correct shared_ptr serialization depends upon object tracking
    // being used.
    BOOST_STATIC_ASSERT(
        boost::serialization::tracking_level< T >::value
        != boost::serialization::track_never
    );
    boost::serialization::split_free(ar, t, file_version);
}

#endif
}
}
#endif
