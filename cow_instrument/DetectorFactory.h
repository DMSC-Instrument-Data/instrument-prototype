#ifndef DETECTOR_FACTORY
#define DETECTOR_FACTORY

#include <memory>
#include "IdType.h"
class Detector;

class DetectorFactory {

public:
  virtual std::unique_ptr<Detector> create_unique(ComponentIdType componentId,
                                                 DetectorIdType detectorId,
                                                 const V3D &pos) const = 0;

  virtual Detector* create(ComponentIdType componentId,
                                                 DetectorIdType detectorId,
                                                 const V3D &pos) const = 0;

  virtual ~DetectorFactory() {}
};

#endif
