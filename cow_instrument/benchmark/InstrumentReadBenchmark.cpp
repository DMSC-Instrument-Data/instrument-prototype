#include "StandardInstrument.h"
#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"

#include <benchmark/benchmark_api.h>

namespace {

class ReadFixture : public benchmark::Fixture {
public:
  InstrumentTree m_instrument;

  ReadFixture()
      : benchmark::Fixture(),
        m_instrument(std_instrument::construct_root_node(), 60000) {}
};

void BM_InstrumentTreeConstruction(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto flattenedNodeTree = std_instrument::construct_root_node();
    state.ResumeTiming();

    InstrumentTree instrument(std::move(flattenedNodeTree), 60000);
  }
}
BENCHMARK(BM_InstrumentTreeConstruction);

BENCHMARK_F(ReadFixture, BM_SngleAccessMetric)(benchmark::State &state) {
  while (state.KeepRunning()) {
<<<<<<< Updated upstream
    state.PauseTiming();
    InstrumentTree instrument(std_instrument::construct_root_node(), 60000);
    state.ResumeTiming();

    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = instrument.getDetector(i);
      pos_x = det.getPos()[0];
    }
  }
}
BENCHMARK(BM_SingleAccessMetrics);

void BM_SingleAccessMetricsStatic(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    static InstrumentTree instrument(std_instrument::construct_root_node(),
                                     60000);
    state.ResumeTiming();
=======
>>>>>>> Stashed changes

    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = m_instrument.getDetector(i);
      pos_x = det.getPos()[0];
    }
  }
}

} // namespace

BENCHMARK_MAIN()
