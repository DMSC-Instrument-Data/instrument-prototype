#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "MoveCommand.h"
#include <iostream>
#include <benchmark/benchmark_api.h>

namespace {

/*
 * Helper function to perform the benchmark at the specified deth in the
 * Instrument tree.
*/
void modifyNodeAtDepth(size_t depth, benchmark::State &state) {

  int countVersion = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    InstrumentTree instrument(std_instrument::construct_root_node(), 60000);
    // Walk down the tree to depth
    const Node *node = &instrument.root();
    for (size_t i = 0; i < depth; ++i) {
      node = &node->child(0);
    }
    MoveCommand moveIt{V3D{1, 0, 0}};
    state.ResumeTiming();

    // Then modify that node.
    Node_const_uptr copyNode = node->modify(moveIt);
    // Then create the instrument around that node
    InstrumentTree copyTree(std::move(copyNode), instrument.nDetectors());
    countVersion += copyTree.version();
  }
  state.SetItemsProcessed(state.iterations() * 1);
}

void BM_copy_unmodified(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    InstrumentTree instrument(std_instrument::construct_root_node(), 60000);
    state.ResumeTiming();

    // Time an unmodified copy.
    InstrumentTree copy = instrument;
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
BENCHMARK(BM_copy_unmodified);

void BM_copy_move_root(benchmark::State &state) {
  modifyNodeAtDepth(0 /*down to the root*/, state);
}
BENCHMARK(BM_copy_move_root);

void BM_copy_move_one_trolley(benchmark::State &state) {
  modifyNodeAtDepth(1 /*down to the trolley*/, state);
}
BENCHMARK(BM_copy_move_one_trolley);

void BM_copy_move_one_bank(benchmark::State &state) {
  modifyNodeAtDepth(2 /*down to the bank*/, state);
}
BENCHMARK(BM_copy_move_one_bank);

} // namespace

// BENCHMARK_MAIN()
