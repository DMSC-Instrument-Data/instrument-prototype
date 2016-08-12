#ifndef _VECTOROFCOMPONENTMAPPER_H
#define _VECTOROFCOMPONENTMAPPER_H

#include "Component.h"
#include "VectorOfMapper.h"
#include <vector>
#include <memory>

namespace {

const std::vector<Component *>
asRawPointers(const std::vector<std::unique_ptr<Component>> &source) {

  std::vector<Component *> product(source.size());
  size_t i = 0;
  for (const auto &item : source) {
    /*
     This might not be ideal. You could consider it unsafe. The only other way I
     see to handle this is to change the storage
     type in ComponsiteComponent to be std::vector<std::shared_ptr<Component> >
     and then
     create a vector of weak pointers from it as part of this mapper.
    */
    product[i] = item.get();
    ++i;
  }
  return product;
}
}

class VectorOfComponentMapper : public VectorOfMapper<Component *> {
public:
  VectorOfComponentMapper() =
      default; // VectorOfMapper<Component *>::VectorOfMapper;

  VectorOfComponentMapper(const std::vector<std::unique_ptr<Component>> &source)
      : VectorOfMapper<Component *>(asRawPointers(source)){

        };

  void store(const std::vector<std::unique_ptr<Component>> &source) {
    heldValue = asRawPointers(source);
  }
};

#endif
