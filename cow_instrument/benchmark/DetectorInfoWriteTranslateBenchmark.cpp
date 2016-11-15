#include "StandardInstrument.h"
#include "DetectorInfo.h"
#include <benchmark/benchmark_api.h>

namespace {

class DetectorInfoWriteTranslateFixture : public StandardInstrumentFixture {

public:
  void translateOnComponent(size_t componentIndex, bool read,
                            benchmark::State &state) {
    Eigen::Vector3d offset{1, 0, 0};

    while (state.KeepRunning()) {
      // Then modify that node
      m_detectorInfo.move(componentIndex, offset);

      // If we want to compare reads too.
      if (read) {
        Eigen::Vector3d pos;
        size_t nComponents =
            m_detectorInfo.const_instrumentTree().componentSize();
        for (size_t i = 0; i < nComponents; ++i) {
          benchmark::DoNotOptimize(pos += m_detectorInfo.position2(i));
        }
      }
    }
    // For statistics. Mark the number of itertions
    state.SetItemsProcessed(state.iterations() * 1);
  }
};

BENCHMARK_F(DetectorInfoWriteTranslateFixture,
            BM_translate_root)(benchmark::State &state) {
  this->translateOnComponent(0, false /*no read metric*/, state);
}

BENCHMARK_F(DetectorInfoWriteTranslateFixture,
            BM_translate_one_trolley)(benchmark::State &state) {
  this->translateOnComponent(1, false /*no read metric*/, state);
}

BENCHMARK_F(DetectorInfoWriteTranslateFixture,
            BM_translate_one_bank)(benchmark::State &state) {
  this->translateOnComponent(2, false /*no read metric*/, state);
}

BENCHMARK_F(DetectorInfoWriteTranslateFixture,
            BM_translate_one_bank_with_read)(benchmark::State &state) {
  this->translateOnComponent(2, true, state);
}

} // namespace
