#ifndef SOURCE_SAMPLE_DETECTOR_PATH_FACTORY_TEST_H
#define SOURCE_SAMPLE_DETECTOR_PATH_FACTORY_TEST_H

#include "PathFactory.h"
#include "Path.h"

/**
 * All Paths generated for all detectors in this PathFactory type
 * will go source->sample->detector.
 */
class SourceSampleDetectorPathFactory : public PathFactory {

public:
  Paths createL2(const InstrumentTree &instrument) const;

  Paths createL1(const InstrumentTree &instrument) const;
};

#endif
