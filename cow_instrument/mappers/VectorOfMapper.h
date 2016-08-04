#ifndef _V3DMapper_H
#define _V3DMapper_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "SingleItemMapper.h"

/**
 ElementType must be serializeable
 */
template <typename ElementType>
class VectorOfMapper : public SingleItemMapper<std::vector<ElementType>> {

public:
  using SingleItemMapper<std::vector<ElementType>>::SingleItemMapper;

  virtual ~VectorOfMapper() {}
};

#endif
