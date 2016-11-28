#include "benchmark/benchmark_api.h"
#include "BenchmarkPolicy.h"
#include "FlatTree.h"
#include "SpectrumInfo.h"
#include "StandardInstrument.h"
#include "SourceSampleDetectorPathFactory.h"
#include <memory.h>

namespace {

class SpectrumInfoFixture : public BenchmarkPolicy<SpectrumInfoFixture> {

public:
  SpectrumInfo<FlatTree> m_spectrumInfo;

  SpectrumInfoFixture()
      : BenchmarkPolicy(), m_spectrumInfo(DetectorInfo<FlatTree>(
                               std::make_shared<FlatTree>(
                                   std_instrument::construct_root_component()),
                               SourceSampleDetectorPathFactory<FlatTree>{})) {}
};

BENCHMARK_F(SpectrumInfoFixture, BM_l2_access)(benchmark::State &state) {
  double l2 = 0;
  while (state.KeepRunning()) {
    // Loop over all spectrum and extract all L2 values
    for (size_t i = 0; i < m_spectrumInfo.size(); ++i) {
      benchmark::DoNotOptimize(l2 += m_spectrumInfo.l2(i));
    }

  }
  state.SetItemsProcessed(state.iterations() * 1);
}
}
