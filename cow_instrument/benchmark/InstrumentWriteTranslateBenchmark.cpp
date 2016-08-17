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

BENCHMARK_F(StandardInstrumentFixture, BM_cow_translate_root)(benchmark::State &state) {
  MoveCommand moveIt{V3D{1, 0, 0}};
  auto temp = m_instrument;
  while (state.KeepRunning()) {
    // Then modify that node
    temp.modify(size_t(0), moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(StandardInstrumentFixture,
            BM_cow_translate_one_trolley)(benchmark::State &state) {
    auto temp = m_instrument;
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    temp.modify(1, moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(StandardInstrumentFixture, BM_cow_translate_one_bank)(benchmark::State &state) {
  auto temp = m_instrument;
  MoveCommand moveIt{V3D{1, 0, 0}};
  while (state.KeepRunning()) {
    // Then modify that node
    temp.modify(2, moveIt);
  }
  // For statistics. Mark the number of itertions
  state.SetItemsProcessed(state.iterations() * 1);
}

} // namespace

