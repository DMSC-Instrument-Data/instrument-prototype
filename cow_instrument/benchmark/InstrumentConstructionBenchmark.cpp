#include "StandardBenchmark.h"
#include "StandardInstrument.h"
#include <benchmark/benchmark.h>

namespace {

class InstrumentConstructionBenchmark
    : public StandardBenchmark<InstrumentConstructionBenchmark> {};

BENCHMARK_F(InstrumentConstructionBenchmark,
            BM_instrument_component_tree_construction)(
    benchmark::State &state) {

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(std_instrument::construct_root_component());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(InstrumentConstructionBenchmark,
            BM_instrument_tree_copy_construction)(benchmark::State &state) {
  size_t nDetectors = 0;
  while (state.KeepRunning()) {
    auto tree = std_instrument::construct_root_component();
    FlatTree instrumentTree(tree);
    benchmark::DoNotOptimize(nDetectors += instrumentTree.nDetectors());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
}
