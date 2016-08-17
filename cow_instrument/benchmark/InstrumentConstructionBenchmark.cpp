#include "StandardInstrument.h"
#include "Node.h"
#include <benchmark/benchmark_api.h>

namespace {

void BM_instrument_node_tree_construction(benchmark::State &state) {
  unsigned int size = 0;
  while (state.KeepRunning()) {
    auto flattenedNodeTree = std_instrument::construct_root_node();
    benchmark::DoNotOptimize(size += flattenedNodeTree.size());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_instrument_node_tree_construction);

void BM_instrument_tree_construction(benchmark::State &state) {
  unsigned int version = 0;
  while (state.KeepRunning()) {
    auto flattenedNodeTree = std_instrument::construct_root_node();
    InstrumentTree instrumentTree(std::move(flattenedNodeTree));
    benchmark::DoNotOptimize(version += instrumentTree.version());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_instrument_tree_construction);

BENCHMARK_F(StandardInstrumentFixture, BM_instrument_tree_copy_unmodified)(benchmark::State &state) {
    while (state.KeepRunning()) {
      // Then modify that node
      auto copyInstrument = m_instrument;
    }
    // For statistics. Mark the number of itertions
    state.SetItemsProcessed(state.iterations() * 1);
  }

}
