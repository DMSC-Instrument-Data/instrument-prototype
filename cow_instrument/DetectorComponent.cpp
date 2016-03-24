#include "DetectorComponent.h"

DetectorComponent::DetectorComponent(ComponentIdType componentId,
                                     DetectorIdType detectorId, const V3D &pos, size_t detectorIndex)
    : m_componentId(componentId), m_detectorId(detectorId), m_pos(pos), m_detectorIndex(detectorIndex) {}

V3D DetectorComponent::getPos() const { return m_pos; }

void DetectorComponent::deltaPos(const V3D &delta) {
  m_pos[0] += delta[0];
  m_pos[1] += delta[1];
  m_pos[2] += delta[2];
}

DetectorComponent *DetectorComponent::clone() const {

  return new DetectorComponent(this->m_componentId, this->m_detectorId,
                               this->m_pos, m_detectorIndex);
}

bool DetectorComponent::equals(const Component &other) const {
  if (auto *otherDetector = dynamic_cast<const DetectorComponent *>(&other)) {
    // Comparision is only based on ID. Index is assumed to be auxillary.
    return otherDetector->detectorId() == this->detectorId();
  }
  return false;
}

void DetectorComponent::registerContents(
    std::map<size_t, const Detector *> &lookup) const {
    lookup.insert(std::make_pair(this->detectorId().const_ref(), this));
}

size_t DetectorComponent::detectorIndex() const
{
    return m_detectorIndex;
}

DetectorComponent::~DetectorComponent() {}

DetectorIdType DetectorComponent::detectorId() const { return m_detectorId; }

ComponentIdType DetectorComponent::componentId() const { return m_componentId; }
