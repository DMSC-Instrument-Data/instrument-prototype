#ifndef _V3DMapper_H
#define _V3DMapper_H

#include <boost/serialization/serialization.hpp>
#include "SingleItemMapper.h"
#include <Eigen/Core>
#include "ArraySerialization.h"


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
