#ifndef SOURCE_SAMPLE_DETECTOR_PATH_FACTORY_TEST_H
#define SOURCE_SAMPLE_DETECTOR_PATH_FACTORY_TEST_H

#include "PathFactory.h"
#include "Path.h"
#include <vector>

/**
 * All Paths generated for all detectors in this PathFactory type
 * will go source->sample->detector.
 */
template <typename InstTree>
class SourceSampleDetectorPathFactory : public PathFactory<InstTree> {

public:
  Paths *createL2(const InstTree &instrument) const;

  Paths *createL1(const InstTree &instrument) const;
};

template <typename InstTree>
Paths *SourceSampleDetectorPathFactory<InstTree>::createL2(
    const InstTree &instrument) const {

  const size_t sampleIndex = instrument.samplePathIndex();
  std::vector<Path> paths;
  paths.reserve(instrument.nDetectors());

  for (size_t i = 0; i < instrument.nDetectors(); ++i) {
    paths.emplace_back(Path(1, sampleIndex));
  }
  return new Paths(std::move(paths));
}

template <typename InstTree>
Paths *SourceSampleDetectorPathFactory<InstTree>::createL1(
    const InstTree &instrument) const {
  const size_t sampleIndex = instrument.samplePathIndex();
  const size_t sourceIndex = instrument.sourcePathIndex();
  return new Paths(instrument.nDetectors(), Path{sourceIndex, sampleIndex});
}

#endif
