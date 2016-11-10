#ifndef STANDARD_INSTRUMENT_H
#define STANDARD_INSTRUMENT_H

#include "InstrumentTree.h"
#include "DetectorInfo.h"
#include <vector>
#include <benchmark/benchmark_api.h>

class Node;
namespace std_instrument {
std::vector<Node> construct_root_node();
}

/*
 Create a standard instrument fixture.
 */
class StandardInstrumentFixture : public benchmark::Fixture {

public:
  InstrumentTree m_instrument;
  DetectorInfo<InstrumentTree> m_detectorInfo;

  StandardInstrumentFixture();
};

#endif
