#include "StandardInstrument.h"
#include <benchmark/benchmark.h>
#include <stdexcept>
#include <numeric>

namespace {

class DetectorInfoWriteTranslateFixture : public StandardInstrumentFixture {

public:
  void moveBankOneDetector(benchmark::State &state) {

    Eigen::Vector3d offset{1, 0, 0};

    while (state.KeepRunning()) {
      // Then modify that node

      m_detectorInfo.moveDetector(0, offset);

      // For statistics. Mark the number of itertions
      state.SetItemsProcessed(state.iterations() * 1);
    }
  }

  void moveDetectorsAsBatch(benchmark::State &state) {

    std::vector<size_t> detectorIndexes(m_detectorInfo.detectorSize());
    std::iota(detectorIndexes.begin(), detectorIndexes.end(), 0);

    Eigen::Vector3d offset{1, 0, 0};

    while (state.KeepRunning()) {
      // Then modify that node

      m_detectorInfo.moveDetectors(detectorIndexes, offset);

      // For statistics. Mark the number of itertions
      state.SetItemsProcessed(state.iterations() *
                              m_detectorInfo.detectorSize());
    }
  }
};

BENCHMARK_F(DetectorInfoWriteTranslateFixture,
            BM_translate_bank_one_by_one)(benchmark::State &state) {
  this->moveBankOneDetector(state);
}

BENCHMARK_F(DetectorInfoWriteTranslateFixture,
            BM_translate_bank_at_once)(benchmark::State &state) {
  this->moveDetectorsAsBatch(state);
}

} // namespace
