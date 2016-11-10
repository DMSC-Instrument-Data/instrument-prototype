#include "StandardInstrument.h"
#include <benchmark/benchmark_api.h>

namespace {

BENCHMARK_F(StandardInstrumentFixture,
            BM_detectorinfo_component_read_single_position)(
    benchmark::State &state) {
  const size_t max = m_detectorInfo.componentSize();
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      benchmark::DoNotOptimize(m_detectorInfo.position2(i));
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

BENCHMARK_F(StandardInstrumentFixture,
            BM_detectorinfo_component_read_single_rotation)(
    benchmark::State &state) {
  const size_t max = m_detectorInfo.componentSize();
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      benchmark::DoNotOptimize(m_detectorInfo.rotation(i));
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

BENCHMARK_F(StandardInstrumentFixture,
            BM_detectorinfo_detector_read_single_position)(
    benchmark::State &state) {
  const size_t max = m_detectorInfo.size(); // ndetectors
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      benchmark::DoNotOptimize(m_detectorInfo.positionDetector(i));
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

BENCHMARK_F(StandardInstrumentFixture,
            BM_detectorinfo_detector_read_single_rotation)(
    benchmark::State &state) {
  const size_t max = m_detectorInfo.size(); // ndetectors
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      benchmark::DoNotOptimize(m_detectorInfo.rotationDetector(i));
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

} // namespace

BENCHMARK_MAIN()
