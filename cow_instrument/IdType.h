#ifndef ID_TYPES_H
#define ID_TYPES_H

#include <array>
#include "IntToType.h"

using DetectorIdType = IntToType<1, size_t>;
using ComponentIdType = IntToType<2, size_t>;
using V3D = std::array<double, 3>; // TODO Move to own header

#endif
