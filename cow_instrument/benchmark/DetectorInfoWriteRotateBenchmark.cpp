#include "StandardInstrument.h"
#include <benchmark/benchmark_api.h>
#include <stdexcept>
#include <numeric>

namespace {

class DetectorInfoWriteRotateFixture : public StandardInstrumentFixture {

public:
  void rotateOneDetector(benchmark::State &state) {

    Eigen::Vector3d axis{0, 0, 1};
    auto angle = M_PI / 2;
    Eigen::Vector3d center{0, 0, 0};

    while (state.KeepRunning()) {
      // Then modify that node

      m_detectorInfo.rotateDetector(0, axis, angle, center);

      // For statistics. Mark the number of itertions
      state.SetItemsProcessed(state.iterations() * 1);
    }
  }

  void rotateDetectorsAsBatch(benchmark::State &state) {

    std::vector<size_t> detectorIndexes(m_detectorInfo.detectorSize());
    std::iota(detectorIndexes.begin(), detectorIndexes.end(), 0);

    Eigen::Vector3d axis{0, 0, 1};
    auto angle = M_PI / 2;
    Eigen::Vector3d center{0, 0, 0};

    while (state.KeepRunning()) {
      // Then modify that node

      m_detectorInfo.rotateDetectors(detectorIndexes, axis, angle, center);

      // For statistics. Mark the number of itertions
      state.SetItemsProcessed(state.iterations() *
                              m_detectorInfo.detectorSize());
    }
  }
};

BENCHMARK_F(DetectorInfoWriteRotateFixture,
            BM_rotate_bank_one_by_one)(benchmark::State &state) {
  this->rotateOneDetector(state);
}

BENCHMARK_F(DetectorInfoWriteRotateFixture,
            BM_rotate_bank_at_once)(benchmark::State &state) {
  this->rotateDetectorsAsBatch(state);
}

} // namespace
