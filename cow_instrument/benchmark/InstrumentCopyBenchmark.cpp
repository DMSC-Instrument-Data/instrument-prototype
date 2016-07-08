#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Command.h"
#include "Detector.h"
#include "MoveCommand.h"
#include <iostream>
#include <benchmark/benchmark_api.h>
#include "IdType.h"

namespace {

class NullCommand : public Command {
public:
  bool execute(CowPtr<Component> &) const {
    return false; // Writes nothing
  }
  bool isMetaDataCommand() const { return false; /*keep cascading*/ }
  virtual ~NullCommand() = default;
};

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

using CopyAtRootLevel = CopyFixture<0>;
using CopyAtTrolleyLevel = CopyFixture<1>;
using CopyAtBankLevel = CopyFixture<2>;

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
    m_instrument.modify(size_t(0), moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(CopyAtTrolleyLevel,
            BM_copy_move_one_trolley)(benchmark::State &state) {
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    m_instrument.modify(1, moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(CopyAtBankLevel, BM_copy_move_one_bank)(benchmark::State &state) {
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    m_instrument.modify(2, moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(CopyAtRootLevel,
            BM_copy_null_command_root)(benchmark::State &state) {
  NullCommand null;
  while (state.KeepRunning()) {
    // Then modify that node
    m_instrument.modify(size_t(0), null);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

} // namespace

// BENCHMARK_MAIN()
