#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "Component.h"
#include "FlatTreeMapper.h"
#include "FlatTree.h"

FlatTreeMapper::FlatTreeMapper() {}

FlatTreeMapper::FlatTreeMapper(const FlatTree &source) { store(source); }

FlatTree FlatTreeMapper::create() {
  if (componentMapper.initializedWithSource()) {
    return FlatTree(std::shared_ptr<Component>(componentMapper.create()));
  } else {
    throw std::invalid_argument("InstrumentTreeMapper unable to deserialize "
                                "via create because members are uninitialized");
  }
}

void FlatTreeMapper::store(const FlatTree &source) {
  componentMapper.storeSink(source.rootComponent());
}

template <class Archive>
void FlatTreeMapper::serialize(Archive &ar, const unsigned int version) {
  boost::serialization::serialize(ar, componentMapper, version);
}

template void FlatTreeMapper::serialize(boost::archive::text_iarchive &ar,
                                        const unsigned int version);
template void FlatTreeMapper::serialize(boost::archive::text_oarchive &ar,
                                        const unsigned int version);
