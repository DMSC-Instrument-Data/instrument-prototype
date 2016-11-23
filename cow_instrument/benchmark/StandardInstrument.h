#ifndef STANDARD_INSTRUMENT_H
#define STANDARD_INSTRUMENT_H

#include "FlatTree.h"
#include "DetectorInfo.h"
#include <vector>
#include <benchmark/benchmark_api.h>

class Node;
namespace std_instrument {
std::shared_ptr<Component> construct_root_component();
}

/*
 Create a standard instrument fixture.
 */
class StandardInstrumentFixture : public benchmark::Fixture {

public:
  FlatTree m_instrument;
  DetectorInfo<FlatTree> m_detectorInfo;

  StandardInstrumentFixture();
};

#endif
