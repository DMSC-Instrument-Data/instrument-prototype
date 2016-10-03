#ifndef INSTRUMENTTREEMAPPER_H
#define INSTRUMENTTREEMAPPER_H

#include "NodeMapper.h"
#include "VectorOfMapper.h"
#include <memory>


class InstrumentTreeMapper {
  public:

    std::shared_ptr<VectorOfMapper<NodeMapper>> nodeMapper;

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
