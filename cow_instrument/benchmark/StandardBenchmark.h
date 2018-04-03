#ifndef STANDARDBENCHMARK_H
#define STANDARDBENCHMARK_H

#include <benchmark/benchmark.h>

/**
 * CRTP For providing a global benchmark execution policy set.
 */
template <typename T> class StandardBenchmark : public benchmark::Fixture {
public:
  StandardBenchmark();
};

template <typename T>
StandardBenchmark<T>::StandardBenchmark()
    : benchmark::Fixture() {
  // Every benchmark repeated 6 times.
  Repetitions(6);
  // Only report the mean and standard deviations
  ReportAggregatesOnly(true);
}

#endif
