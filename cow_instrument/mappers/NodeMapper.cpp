#include "NodeMapper.h"
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "SharedPtrSerialization.h"

template <class Archive>
void NodeMapper::serialize(Archive &ar, const unsigned int version) {
  using namespace boost::serialization;
  boost::serialization::serialize(ar, previousMapper, version);
  boost::serialization::serialize(ar, contentsMapper, version);
  boost::serialization::serialize(ar, nameMapper, version);
  boost::serialization::serialize(ar, nextIndexMapper, version);
  boost::serialization::serialize(ar, versionMapper, version);
}

NodeMapper::NodeMapper()
    : contentsMapper(std::make_shared<ComponentMapper>()) {}

NodeMapper::NodeMapper(const Node &source)
    : contentsMapper(std::make_shared<ComponentMapper>()) {
  store(source);
}

Node NodeMapper::create() {
  if (previousMapper.initalized() && contentsMapper->initialized() &&
      nameMapper.initalized() && nextIndexMapper.initalized() &&
      versionMapper.initalized()) {
    return Node(previousMapper.create(), contentsMapper->create(),
                nameMapper.create(), versionMapper.create(),
                nextIndexMapper.create());
  } else {
    throw std::invalid_argument("Cannot be deserialized. Not all mandatory "
                                "construction fields have been provided for "
                                "Node");
  }
}

void NodeMapper::store(const Node &source) {
  previousMapper = source.parent();
  contentsMapper->store(source.unsafeContents().get()); // HACK.
  nameMapper = source.name();
  nextIndexMapper = source.children();
  versionMapper = source.version();
}

template void NodeMapper::serialize(boost::archive::text_oarchive &ar,
                                    const unsigned int version);
template void NodeMapper::serialize(boost::archive::text_iarchive &ar,
                                    const unsigned int version);
