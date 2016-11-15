#include "StandardInstrument.h"
#include <benchmark/benchmark_api.h>
#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "SourceSampleDetectorPathFactory.h"
#include <memory>

namespace {

class DetectorInfoCreationFixture : public StandardInstrumentFixture {
private:
  std::shared_ptr<InstrumentTree> m_sharedInstrument;

public:
  DetectorInfoCreationFixture()
      : m_sharedInstrument(std::make_shared<InstrumentTree>(
            std_instrument::construct_root_component())) {}

  void construct() {
    DetectorInfo<InstrumentTree> original(
        m_sharedInstrument, SourceSampleDetectorPathFactory<InstrumentTree>{});
    (void)original;
  }

  void copy() {
    DetectorInfo<InstrumentTree> copy(m_detectorInfo);
    (void)copy;
  }
};

BENCHMARK_F(DetectorInfoCreationFixture,
            BM_detector_info_construction)(benchmark::State &state) {

  while (state.KeepRunning()) {
    this->construct();
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK_F(DetectorInfoCreationFixture,
            BM_detector_info_copy_construction)(benchmark::State &state) {
  while (state.KeepRunning()) {
    copy();
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
}
