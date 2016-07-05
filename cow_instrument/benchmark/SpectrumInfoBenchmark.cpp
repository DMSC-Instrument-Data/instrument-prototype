#include "benchmark/benchmark_api.h"
#include "InstrumentTree.h"
#include "SpectrumInfo.h"
#include "StandardInstrument.h"
#include "Node.h"
#include <memory.h>

namespace {

class SpectrumInfoFixture : public benchmark::Fixture {

public:
  SpectrumInfo<InstrumentTree> m_spectrumInfo;

  SpectrumInfoFixture()
      : benchmark::Fixture(),
        m_spectrumInfo(
            DetectorInfo<InstrumentTree>(std::make_shared<InstrumentTree>(
                std_instrument::construct_root_node(), 6000))) {}
};

BENCHMARK_F(SpectrumInfoFixture, BM_l2_access)(benchmark::State &state) {
  double l2 = 0;
  while (state.KeepRunning()) {
    // Loop over all spectrum and extract all L2 values
    for (size_t i = 0; i < m_spectrumInfo.size(); ++i) {
      benchmark::DoNotOptimize(l2 += m_spectrumInfo.l2(i));
    }

  }
  state.SetItemsProcessed(state.iterations() * m_spectrumInfo.size());
}
}
