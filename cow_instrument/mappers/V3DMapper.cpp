#include "V3DMapper.h"

V3DMapper::V3DMapper(const Eigen::Vector3d &mapee)
    : SingleItemMapper(std::array<double, 3>{mapee[0], mapee[1], mapee[2]}) {}

Eigen::Vector3d V3DMapper::create() const {
  if (heldValue.is_initialized()) {
    auto arry = *heldValue;
    return Eigen::Vector3d(arry[0], arry[1], arry[2]);
  } else {
    throw std::invalid_argument("V3DMapper::create failed. Mapper not "
                                "provided with a serialization item");
  }
}

void V3DMapper::store(const Eigen::Vector3d &mapee) {
  heldValue = std::array<double, 3>{mapee[0], mapee[1], mapee[2]};
}

V3DMapper::~V3DMapper() {}
