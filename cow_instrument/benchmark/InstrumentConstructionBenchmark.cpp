#include "BenchmarkPolicy.h"
#include "StandardInstrument.h"
#include <benchmark/benchmark_api.h>

namespace {

BENCHMARK_F(BenchmarkPolicy, BM_instrument_component_tree_construction)(
    benchmark::State &state) {

  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(std_instrument::construct_root_component());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(BenchmarkPolicy,
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
