#include "SourceSampleDetectorPathFactory.h"
#include "InstrumentTree.h"
#include <vector>

Paths SourceSampleDetectorPathFactory::createL2(
    const InstrumentTree &instrument) const {

  const size_t sampleIndex = instrument.samplePathIndex();
  std::vector<Path> paths;
  paths.reserve(instrument.nDetectors());

  for (size_t i = 0; i < instrument.nDetectors(); ++i) {
    paths.emplace_back(Path{sampleIndex, i});
  }
  return Paths(std::move(paths));
}

Paths SourceSampleDetectorPathFactory::createL1(
    const InstrumentTree &instrument) const {
  const size_t sampleIndex = instrument.samplePathIndex();
  const size_t sourceIndex = instrument.sourcePathIndex();
  return Paths(instrument.nDetectors(), Path{sourceIndex, sampleIndex});
}
