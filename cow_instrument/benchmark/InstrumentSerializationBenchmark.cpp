#include "StandardInstrument.h"
#include "FlatTree.h"
#include "FlatTreeMapper.h"
#include "StandardBenchmark.h"
#include <benchmark/benchmark_api.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <sstream>

namespace {

BENCHMARK_F(StandardInstrumentFixture,
            BM_instrument_tree_serialize)(benchmark::State &state) {

  while (state.KeepRunning()) {
    FlatTreeMapper originalMapper(m_instrument);

    // Write it out
    std::stringstream s;
    boost::archive::text_oarchive out(s);
    benchmark::DoNotOptimize(out << originalMapper);
  }
  state.SetItemsProcessed(state.iterations() * 1);
}

BENCHMARK_F(StandardInstrumentFixture,
            BM_instrument_tree_serialize_and_deserialize)(
    benchmark::State &state) {

  while (state.KeepRunning()) {
    FlatTreeMapper originalMapper(m_instrument);

    // Write it out
    std::stringstream s;
    boost::archive::text_oarchive out(s);
    out << originalMapper;

    // Read it back in
    boost::archive::text_iarchive in(s);
    FlatTreeMapper outputMapper;
    in >> outputMapper;

    benchmark::DoNotOptimize(outputMapper.create());
  }
  state.SetItemsProcessed(state.iterations() * 1);
}
}
