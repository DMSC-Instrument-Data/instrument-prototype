#ifndef INSTRUMENTTREEMAPPER_H
#define INSTRUMENTTREEMAPPER_H

#include "VectorOfMapper.h"
#include "ComponentMapper.h"
#include <memory>


class InstrumentTreeMapper {
  public:
    ComponentMapper componentMapper;
    InstrumentTreeMapper();
    InstrumentTreeMapper(const class InstrumentTree& source);
    InstrumentTree create();
    void store(const InstrumentTree &source);

  private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version);
};

#endif
