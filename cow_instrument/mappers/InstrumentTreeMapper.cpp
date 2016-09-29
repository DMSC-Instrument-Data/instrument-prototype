#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "InstrumentTreeMapper.h"
#include "InstrumentTree.h"

InstrumentTreeMapper::InstrumentTreeMapper() : nodeMapper(new VectorOfMapper<NodeMapper>())
{

}

InstrumentTreeMapper::InstrumentTreeMapper(const InstrumentTree &source) : nodeMapper(new VectorOfMapper<NodeMapper>())
{
    store(source);
}

InstrumentTree InstrumentTreeMapper::create()
{
    if(nodeMapper->isInitialized()){
        std::vector<NodeMapper> nodeMappers = nodeMapper->create();
        std::vector<Node> nodes;
        for(auto& nodeMapper : nodeMappers ){
            nodes.emplace_back(nodeMapper.create());
        }
        return InstrumentTree(std::move(nodes));
    } else {
        throw std::invalid_argument("InstrumentTreeMapper unable to deserialize via create because members are uninitialized");
    }
}

void InstrumentTreeMapper::store(const InstrumentTree &source)
{
    auto nodes = source.unsafeContents();
    std::vector<NodeMapper> contents;

    for(size_t i = 0; i < source.nodeSize(); ++i){
        contents.emplace_back((*nodes)[i]);
    }
    nodeMapper->store(contents);
}

template <class Archive>
void InstrumentTreeMapper::serialize(Archive &ar, const unsigned int version){
    boost::serialization::serialize(ar, nodeMapper, version);
}

template void InstrumentTreeMapper::serialize(boost::archive::text_iarchive &ar,
                                    const unsigned int version);
template void InstrumentTreeMapper::serialize(boost::archive::text_oarchive &ar,
                                    const unsigned int version);

