#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "MoveCommand.h"
#include <iostream>
#include <benchmark/benchmark_api.h>
#include "IdType.h"

namespace {

/*
 * Helper function to perform the benchmark at the specified deth in the
 * Instrument tree.
*/
void modifyNodeAtDepth(size_t depth, bool recordInstrumentCreation,
                       benchmark::State &state) {

  // int counter = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    InstrumentTree instrument(std_instrument::construct_root_node(), 60000);
    // Walk down the tree to depth
    size_t nodeIndex = 0;
    const Node *node = &instrument.root();
    for (size_t i = 0; i < depth; ++i) {
      nodeIndex = node->child(0);
      node = instrument.nodeAt(i);
    }
    MoveCommand moveIt{V3D{1, 0, 0}};
    state.ResumeTiming();

    // Then create the instrument around that node
    if (!recordInstrumentCreation) {
      state.PauseTiming();
    }
    // Then modify that node
    InstrumentTree copyInstrument = instrument.modify(nodeIndex, moveIt);

    if (!recordInstrumentCreation) {
      state.ResumeTiming();
    }
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

// ------------ Timings to include instrument tree creation ------------- //
void
BM_copy_move_root_with_instrument_creation_timing(benchmark::State &state) {
  modifyNodeAtDepth(0 /*down to the root*/, true /*Time instrument creation*/,
                    state);
}
BENCHMARK(BM_copy_move_root_with_instrument_creation_timing);

void BM_copy_move_one_trolley_with_instrument_creation_timing(
    benchmark::State &state) {
  modifyNodeAtDepth(1 /*down to the trolley*/,
                    true /*Time instrument creation*/, state);
}
BENCHMARK(BM_copy_move_one_trolley_with_instrument_creation_timing);

void
BM_copy_move_one_bank_with_instrument_creation_timing(benchmark::State &state) {
  modifyNodeAtDepth(2 /*down to the bank*/, true /*Time instrument creation*/,
                    state);
}
BENCHMARK(BM_copy_move_one_bank_with_instrument_creation_timing);
// ------------ End Timings to include instrument tree creation ------------- //

// ------------ Timings NOT to include instrument tree creation ------------- //
void
BM_copy_move_root_without_instrument_creation_timing(benchmark::State &state) {
  modifyNodeAtDepth(0 /*down to the root*/, false /*Time instrument creation*/,
                    state);
}
BENCHMARK(BM_copy_move_root_without_instrument_creation_timing);

void BM_copy_move_one_trolley_without_instrument_creation_timing(
    benchmark::State &state) {
  modifyNodeAtDepth(1 /*down to the trolley*/,
                    false /*Time instrument creation*/, state);
}
BENCHMARK(BM_copy_move_one_trolley_without_instrument_creation_timing);

void BM_copy_move_one_bank_without_instrument_creation_timing(
    benchmark::State &state) {
  modifyNodeAtDepth(2 /*down to the bank*/, false /*Time instrument creation*/,
                    state);
}
BENCHMARK(BM_copy_move_one_bank_without_instrument_creation_timing);
// ------------ End Timings NOT to include instrument tree creation
// ------------- //

} // namespace

// BENCHMARK_MAIN()
