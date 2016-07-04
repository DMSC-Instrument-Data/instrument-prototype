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
 * Templated CopyFixture
*/
template <size_t depth> class CopyFixture : public benchmark::Fixture {

public:
  InstrumentTree m_instrument;
  size_t m_nodeIndex;

  CopyFixture()
      : benchmark::Fixture(),
        m_instrument(std_instrument::construct_root_node(), 60000),
        m_nodeIndex(0) {

    // Walk down the tree to depth
    const Node *node = &m_instrument.root();
    for (size_t i = 0; i < depth; ++i) {
      m_nodeIndex = node->child(0);
      node = m_instrument.nodeAt(i);
    }
  }
};

<<<<<<< Updated upstream
    // Then create the instrument around that node
    if (!recordInstrumentCreation) {
      state.PauseTiming();
    }
    // Then modify that node
    InstrumentTree copyInstrument = instrument.modify(nodeIndex, moveIt);
=======
using CopyAtRootLevel = CopyFixture<0>;
using CopyAtTrolleyLevel = CopyFixture<1>;
using CopyAtBankLevel = CopyFixture<2>;
>>>>>>> Stashed changes

BENCHMARK_F(CopyAtRootLevel, BM_copy_unmodified)(benchmark::State &state) {
  while (state.KeepRunning()) {
    // Then modify that node
    auto copyInstrument = m_instrument;
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(CopyAtRootLevel, BM_copy_move_root)(benchmark::State &state) {
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    auto copyInstrument = m_instrument.modify(size_t(0), moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(CopyAtTrolleyLevel,
            BM_copy_move_one_trolley)(benchmark::State &state) {
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    auto copyInstrument = m_instrument.modify(1, moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(CopyAtBankLevel, BM_copy_move_one_bank)(benchmark::State &state) {
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    auto copyInstrument = m_instrument.modify(2, moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

} // namespace

// BENCHMARK_MAIN()
