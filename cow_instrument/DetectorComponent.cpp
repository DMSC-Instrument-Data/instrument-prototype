#include "DetectorComponent.h"

DetectorComponent::DetectorComponent(size_t id, const V3D &pos) : m_id(id), m_pos(pos) {}

V3D DetectorComponent::getPos() const { return m_pos; }

void DetectorComponent::deltaPos(const V3D &delta) {
  m_pos[0] += delta[0];
  m_pos[1] += delta[1];
  m_pos[2] += delta[2];
}

DetectorComponent *DetectorComponent::clone() const {

  return new DetectorComponent(this->m_id, this->m_pos);
}

bool DetectorComponent::equals(const Component &other) const {
  if (auto *otherDetector = dynamic_cast<const DetectorComponent *>(&other)) {
    return otherDetector->id() == this->id();
  }
  return false;
}

void DetectorComponent::registerDetectors(std::map<size_t, const Detector *> &lookup) const
{
    lookup.insert(std::make_pair(this->id(), this));
}

DetectorComponent::~DetectorComponent() {}
