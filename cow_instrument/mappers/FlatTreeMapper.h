#ifndef INSTRUMENTTREEMAPPER_H
#define INSTRUMENTTREEMAPPER_H

#include "VectorOfMapper.h"
#include "ComponentMapper.h"
#include <memory>

class FlatTreeMapper {
public:
  ComponentMapper componentMapper;
  FlatTreeMapper();
  FlatTreeMapper(const class FlatTree &source);
  FlatTree create();
  void store(const FlatTree &source);

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};

#endif
