#include "BenchmarkPolicy.h"

BenchmarkPolicy::~BenchmarkPolicy() {}

BenchmarkPolicy::BenchmarkPolicy() : benchmark::Fixture() {
  // Every benchmark repeated 6 times.
  Repetitions(6);
  // Only report the mean and standard deviations
  ReportAggregatesOnly(true);
}
