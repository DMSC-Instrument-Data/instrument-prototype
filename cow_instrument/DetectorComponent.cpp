#include "DetectorComponent.h"
#include <Eigen/Geometry>

DetectorComponent::DetectorComponent(ComponentIdType componentId,
                                     DetectorIdType detectorId, const V3D &pos)
    : m_componentId(componentId), m_pos(pos), m_detectorId(detectorId), m_rotation(Eigen::Quaterniond::Identity()) {}

V3D DetectorComponent::getPos() const { return m_pos; }

Eigen::Quaterniond DetectorComponent::getRotation() const
{
    return m_rotation;
}

void DetectorComponent::shiftPositionBy(const V3D &delta) {
  m_pos += delta;
}

void DetectorComponent::rotate(const Eigen::Vector3d &axis, const double &theta, const Eigen::Vector3d &center)
{
    using namespace Eigen;
    const Affine3d transform = Translation3d(center) * AngleAxisd(theta, axis) *
                               Translation3d(-center);
    m_pos = transform * m_pos;
    // Update the absolute rotation of this detector around own center.
    m_rotation = transform.rotation() * m_rotation;
}

void DetectorComponent::rotate(const Eigen::Affine3d &transform,
                               const Eigen::Quaterniond &rotationPart) {
  m_pos = transform * m_pos;
  // Update the absolute rotation of this detector around own center.
  m_rotation = rotationPart * m_rotation;
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
