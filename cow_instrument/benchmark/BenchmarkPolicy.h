#ifndef BENCHMARKPOLICY_H
#define BENCHMARKPOLICY_H

#include <benchmark/benchmark_api.h>

/**
 * Base class to define standard behaviour for performance benchmarks
 */
class BenchmarkPolicy : public benchmark::Fixture {
public:
  BenchmarkPolicy();
  virtual ~BenchmarkPolicy();
};

#endif
