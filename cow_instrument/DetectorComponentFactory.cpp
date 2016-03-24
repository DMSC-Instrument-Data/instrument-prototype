#include "DetectorComponentFactory.h"
#include "Detector.h"
#include "DetectorComponent.h"
#include <memory>

DetectorComponentFactory::DetectorComponentFactory() : m_index(0) {}

std::unique_ptr<Detector>
DetectorComponentFactory::create_unique(ComponentIdType componentId,
                                        DetectorIdType detectorId,
                                        const V3D &pos) const {

  return std::unique_ptr<Detector>(
      create(componentId, detectorId, pos));
}

Detector *DetectorComponentFactory::create(ComponentIdType componentId,
                                                 DetectorIdType detectorId,
                                                 const V3D &pos) const {

    return new DetectorComponent(componentId, detectorId, pos, ++m_index);

}
