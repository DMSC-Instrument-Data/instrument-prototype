#ifndef INSTRUMENTTREEMAPPER_H
#define INSTRUMENTTREEMAPPER_H

#include "VectorOfMapper.h"
#include "NodeMapper.h"

class InstrumentTreeMapper {
  public:

    //CowPtr<std::vector<Node>> m_nodes;


    InstrumentTreeMapper() = default;
    InstrumentTreeMapper(const class InstrumentTree& source);
};

#endif
