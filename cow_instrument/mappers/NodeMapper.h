#ifndef NODE_MAPPER_H
#define NODE_MAPPER_H

#include "Node.h"
#include "ComponentMapper.h"
#include "VectorOfMapper.h"
#include "SingleItemMapper.h"
#include <memory.h>

class NodeMapper {

public:
  SingleItemMapper<int64_t> previousMapper;
  VectorOfMapper<size_t> nextIndexMapper;
  std::shared_ptr<ComponentMapper> contentsMapper;
  SingleItemMapper<std::string> nameMapper;
  SingleItemMapper<unsigned int> versionMapper;

  NodeMapper(const Node &source);
  NodeMapper();

  Node create();

  void store(const Node &source);

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version);
};

#endif
