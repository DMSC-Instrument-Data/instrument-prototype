#include "StandardInstrument.h"
#include <benchmark/benchmark.h>

namespace {

using DetectorInfoReadFixture = StandardInstrumentFixture;

BENCHMARK_F(DetectorInfoReadFixture,
            BM_detectorinfo_detector_read_single_position)(
    benchmark::State &state) {
  const size_t max = m_detectorInfo.detectorSize(); // ndetectors
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      benchmark::DoNotOptimize(m_detectorInfo.position(i));
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

BENCHMARK_F(DetectorInfoReadFixture,
            BM_detectorinfo_detector_read_single_rotation)(
    benchmark::State &state) {
  const size_t max = m_detectorInfo.detectorSize(); // ndetectors
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      benchmark::DoNotOptimize(m_detectorInfo.rotation(i));
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

} // namespace

BENCHMARK_MAIN();
