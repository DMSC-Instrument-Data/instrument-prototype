#ifndef STANDARD_INSTRUMENT_H
#define STANDARD_INSTRUMENT_H

#include "FlatTree.h"
#include "ComponentInfo.h"
#include "DetectorInfo.h"
#include "StandardBenchmark.h"
#include <vector>
#include <benchmark/benchmark_api.h>

class Node;
namespace std_instrument {
std::shared_ptr<Component> construct_root_component();
}

/*
 Create a standard instrument fixture.
 */
class StandardInstrumentFixture
    : public StandardBenchmark<StandardInstrumentFixture> {

public:
  FlatTree m_instrument;
  ComponentInfo<FlatTree> m_componentInfo;
  DetectorInfo<FlatTree> m_detectorInfo;

  StandardInstrumentFixture();
};

#endif
