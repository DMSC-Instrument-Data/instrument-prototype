#ifndef DETECTOR_COMPONENT_FACTORY
#define DETECTOR_COMPONENT_FACTORY

#include "DetectorFactory.h"
#include <atomic>

class DetectorComponentFactory : public DetectorFactory {
public:

  DetectorComponentFactory();

  virtual std::unique_ptr<Detector> create_unique(ComponentIdType componentId,
                                           DetectorIdType detectorId,
                                           const V3D &pos) const override;

  virtual Detector * create(ComponentIdType componentId,
                                  DetectorIdType detectorId,
                                  const V3D &pos) const override;

private:

    mutable std::atomic<size_t> m_index;
};

#endif
