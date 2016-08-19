#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "MoveCommand.h"
#include <iostream>
#include <benchmark/benchmark_api.h>
#include "IdType.h"

namespace {

class WriteTranslateFixture : public StandardInstrumentFixture {

public:
  void translateOnNode(size_t nodeIndex, benchmark::State &state) {
    MoveCommand moveIt{Eigen::Vector3d{1, 0, 0}};
    while (state.KeepRunning()) {
      // Then modify that node
      m_instrument.modify(nodeIndex, moveIt);
    }
    // For statistics. Mark the number of itertions
    state.SetItemsProcessed(state.iterations() * 1);
  }

  void translateOnNodeWithCopy(size_t nodeIndex, benchmark::State &state) {
    MoveCommand moveIt{Eigen::Vector3d{1, 0, 0}};
    while (state.KeepRunning()) {
          // increase reference count on components
      auto temp = m_instrument;
      // Then modify that node
      temp.modify(nodeIndex, moveIt);
    }
    // For statistics. Mark the number of itertions
    state.SetItemsProcessed(state.iterations() * 1);
  }
};

BENCHMARK_F(WriteTranslateFixture,
            BM_cow_translate_root)(benchmark::State &state) {
  this->translateOnNodeWithCopy(0, state);
}

BENCHMARK_F(WriteTranslateFixture,
            BM_cow_translate_one_trolley)(benchmark::State &state) {
  this->translateOnNodeWithCopy(1, state);
}

BENCHMARK_F(WriteTranslateFixture,
            BM_cow_translate_one_bank)(benchmark::State &state) {
  this->translateOnNodeWithCopy(2, state);
}

BENCHMARK_F(WriteTranslateFixture, BM_translate_root)(benchmark::State &state) {
  this->translateOnNode(0, state);
}

BENCHMARK_F(WriteTranslateFixture,
            BM_translate_one_trolley)(benchmark::State &state) {
  this->translateOnNode(1, state);
}

BENCHMARK_F(WriteTranslateFixture,
            BM_translate_one_bank)(benchmark::State &state) {
  this->translateOnNode(2, state);
}

} // namespace
