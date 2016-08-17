#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"

#include <benchmark/benchmark_api.h>

namespace {

BENCHMARK_F(StandardInstrumentFixture,
            BM_instrument_tree_read_single_position)(benchmark::State &state) {
  const size_t max = m_instrument.nDetectors();
  while (state.KeepRunning()) {

    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = m_instrument.getDetector(i);
      benchmark::DoNotOptimize(pos_x += det.getPos()[0]);
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

BENCHMARK_F(StandardInstrumentFixture,
            BM_instrument_tree_read_single_rotation)(benchmark::State &state) {
  const size_t max = m_instrument.nDetectors();
  while (state.KeepRunning()) {
    for (size_t i = 1; i < max; ++i) {
      const auto &det = m_instrument.getDetector(i);
      benchmark::DoNotOptimize(det.getRotation());
    }
  }
  state.SetItemsProcessed(state.iterations() * max);
}

} // namespace

BENCHMARK_MAIN()
