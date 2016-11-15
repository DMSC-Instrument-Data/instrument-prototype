#include "StandardInstrument.h"
#include <benchmark/benchmark_api.h>

namespace {

void BM_instrument_component_tree_construction(benchmark::State &state) {

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(std_instrument::construct_root_component());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_instrument_component_tree_construction);

void BM_instrument_tree_copy_construction(benchmark::State &state) {
  size_t nDetectors = 0;
  while (state.KeepRunning()) {
    auto tree = std_instrument::construct_root_component();
    InstrumentTree instrumentTree(tree);
    benchmark::DoNotOptimize(nDetectors += instrumentTree.nDetectors());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_instrument_tree_copy_construction);
}
