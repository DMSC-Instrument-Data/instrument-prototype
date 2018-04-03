#include "StandardInstrument.h"
#include <benchmark/benchmark.h>
#include "DetectorInfo.h"
#include "FlatTree.h"
#include "SourceSampleDetectorPathFactory.h"
#include <memory>

namespace {

class DetectorInfoCreationFixture : public StandardInstrumentFixture {
private:
  std::shared_ptr<FlatTree> m_sharedInstrument;

public:
  DetectorInfoCreationFixture()
      : m_sharedInstrument(std::make_shared<FlatTree>(
            std_instrument::construct_root_component())) {}

  void construct() {
    DetectorInfo<FlatTree> original(
        m_sharedInstrument, SourceSampleDetectorPathFactory<FlatTree>{});
    (void)original;
  }

  void copy() {
    DetectorInfo<FlatTree> copy(m_detectorInfo);
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
