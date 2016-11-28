#ifndef BENCHMARKPOLICY_H
#define BENCHMARKPOLICY_H

#include <benchmark/benchmark_api.h>

/**
 * CRTP For providing a global benchmark execution policy set.
 */
template <typename T> class BenchmarkPolicy : public benchmark::Fixture {
public:
  BenchmarkPolicy();
};

template <typename T>
BenchmarkPolicy<T>::BenchmarkPolicy()
    : benchmark::Fixture() {
  // Every benchmark repeated 6 times.
  Repetitions(6);
  // Only report the mean and standard deviations
  ReportAggregatesOnly(true);
}

#endif
