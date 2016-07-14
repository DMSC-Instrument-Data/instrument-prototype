#include "DetectorComponent.h"

DetectorComponent::DetectorComponent(ComponentIdType componentId,
                                     DetectorIdType detectorId, const V3D &pos)
    : m_componentId(componentId), m_pos(pos), m_detectorId(detectorId) {}

V3D DetectorComponent::getPos() const { return m_pos; }

void DetectorComponent::deltaPos(const V3D &delta) {
  m_pos[0] += delta[0];
  m_pos[1] += delta[1];
  m_pos[2] += delta[2];
}

DetectorComponent *DetectorComponent::clone() const {

  return new DetectorComponent(this->m_componentId, this->m_detectorId,
                               this->m_pos);
}

bool DetectorComponent::equals(const Component &other) const {
  if (auto *otherDetector = dynamic_cast<const DetectorComponent *>(&other)) {
    // Comparision is only based on ID. Index is assumed to be auxillary.
    return otherDetector->detectorId() == this->detectorId();
  }
  return false;
}

void DetectorComponent::registerContents(
    std::vector<const Detector *> &lookupDetectors,
    std::vector<const PathComponent *> &) const {
  lookupDetectors.push_back(this);
}

std::string DetectorComponent::name() const {
  return ""; // Not implmented (yet) for detector component.
}

DetectorComponent::~DetectorComponent() {}

DetectorIdType DetectorComponent::detectorId() const { return m_detectorId; }

ComponentIdType DetectorComponent::componentId() const { return m_componentId; }
