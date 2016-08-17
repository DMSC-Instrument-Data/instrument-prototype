#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"

#include <benchmark/benchmark_api.h>

namespace {


BENCHMARK_F(StandardInstrumentFixture, BM_read_single_position)(benchmark::State &state) {
  while (state.KeepRunning()) {
    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = m_instrument.getDetector(i);
      benchmark::DoNotOptimize(pos_x += det.getPos()[0]);
    }
  }
  state.SetItemsProcessed(state.iterations() * m_instrument.nDetectors());
}

} // namespace

BENCHMARK_MAIN()
