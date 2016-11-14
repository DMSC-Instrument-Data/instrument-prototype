#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Component.h"
#include "InstrumentTreeMapper.h"
#include "InstrumentTree.h"

InstrumentTreeMapper::InstrumentTreeMapper() {}

InstrumentTreeMapper::InstrumentTreeMapper(const InstrumentTree &source) {
    store(source);
}

InstrumentTree InstrumentTreeMapper::create()
{
  if (componentMapper.initializedWithSource()) {
    return InstrumentTree(std::shared_ptr<Component>(componentMapper.create()));
    } else {
        throw std::invalid_argument("InstrumentTreeMapper unable to deserialize via create because members are uninitialized");
    }
}

void InstrumentTreeMapper::store(const InstrumentTree &source)
{
  componentMapper.storeSink(source.rootComponent());
}

template <class Archive>
void InstrumentTreeMapper::serialize(Archive &ar, const unsigned int version){
  boost::serialization::serialize(ar, componentMapper, version);
}

template void InstrumentTreeMapper::serialize(boost::archive::text_iarchive &ar,
                                    const unsigned int version);
template void InstrumentTreeMapper::serialize(boost::archive::text_oarchive &ar,
                                    const unsigned int version);

