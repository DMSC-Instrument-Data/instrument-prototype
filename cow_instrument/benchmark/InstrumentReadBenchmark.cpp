#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"

#include <benchmark/benchmark_api.h>

namespace {

void BM_InstrumentTreeConstruction(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto flattenedNodeTree = std_instrument::construct_root_node();
    state.ResumeTiming();

    InstrumentTree instrument(std::move(flattenedNodeTree), 60000);
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_InstrumentTreeConstruction);

void BM_SingleAccessMetrics(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    InstrumentTree instrument(std_instrument::construct_root_node(), 60000);
    state.ResumeTiming();

    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = instrument.getDetector(i);
      pos_x = det.getPos()[0];
    }
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_SingleAccessMetrics);

void BM_SingleAccessMetricsStatic(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    static InstrumentTree instrument(std_instrument::construct_root_node(),
                                     60000);
    state.ResumeTiming();

    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = instrument.getDetector(i);
      pos_x = det.getPos()[0];
    }
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_SingleAccessMetricsStatic);

} // namespace

BENCHMARK_MAIN()
