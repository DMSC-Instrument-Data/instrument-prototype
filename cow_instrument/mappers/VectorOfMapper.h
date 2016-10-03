#ifndef _V3DMAPPER_H
#define _V3DMAPPER_H

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "SingleItemMapper.h"
#include <vector>

/**
 ElementType must be serializeable
 */
template <typename ElementType>
class VectorOfMapper : public SingleItemMapper<std::vector<ElementType>> {

public:
  using SingleItemMapper<std::vector<ElementType>>::SingleItemMapper;

  virtual ~VectorOfMapper() {}

  size_t size() const {
    if (this->isInitialized()) {
      return this->heldValue->size();
    } else {
      throw std::invalid_argument("VectorOf::size failed. Nothing held.");
    }
  }

  const ElementType &operator[](size_t index) const {
    if (this->isInitialized()) {
      return this->heldValue->operator[](index);
    } else {
      throw std::invalid_argument("VectorOf::operator[] failed. Nothing held.");
    }
  }

};

#endif
