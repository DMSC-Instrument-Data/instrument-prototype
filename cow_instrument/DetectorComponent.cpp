#include "DetectorComponent.h"
#include "ComponentVisitor.h"
#include <Eigen/Geometry>

DetectorComponent::DetectorComponent(ComponentIdType componentId,
                                     DetectorIdType detectorId,
                                     const Eigen::Vector3d &pos)
    : m_componentId(componentId), m_pos(pos), m_detectorId(detectorId),
      m_rotation(Eigen::Quaterniond::Identity()) {}

Eigen::Vector3d DetectorComponent::getPos() const { return m_pos; }

Eigen::Quaterniond DetectorComponent::getRotation() const { return m_rotation; }

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

void DetectorComponent::registerContents(ComponentInfo &info) const {
  info.registerDetector(this);
}

void DetectorComponent::registerContents(ComponentInfo &info,
                                         size_t parentIndex) const {
  info.registerDetector(this, parentIndex);
}

std::string DetectorComponent::name() const {
  return ""; // Not implmented (yet) for detector component.
}

bool DetectorComponent::accept(ComponentVisitor *visitor) const {
  return visitor->visit(this);
}

DetectorComponent::~DetectorComponent() {}

DetectorIdType DetectorComponent::detectorId() const { return m_detectorId; }

ComponentIdType DetectorComponent::componentId() const { return m_componentId; }
