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

void DetectorComponent::shiftPositionBy(const Eigen::Vector3d &delta) {
  m_pos += delta;
}

void DetectorComponent::rotate(const Eigen::Vector3d &axis, const double &theta,
                               const Eigen::Vector3d &center) {
  using namespace Eigen;
  const Affine3d transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
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
    std::vector<const PathComponent *> &, std::vector<size_t> &detectorIndexes,
    std::vector<size_t> &, size_t previousIndex,
    std::vector<ComponentProxy> &componentProxies) const {

  const size_t newIndex = componentProxies.size();
  componentProxies.emplace_back(previousIndex, this);
  componentProxies[previousIndex].addChild(newIndex);
  lookupDetectors.push_back(this);
  detectorIndexes.push_back(newIndex); // We track the thing we just added.
}

void DetectorComponent::registerContents(
    std::vector<const Detector *> &lookupDetectors,
    std::vector<const PathComponent *> &pathLookup,
    std::vector<size_t> &detectorIndexes, std::vector<size_t> &pathIndexes,
    std::vector<ComponentProxy> &componentProxies) const {
  const size_t newIndex = componentProxies.size();
  componentProxies.emplace_back(this);
  lookupDetectors.push_back(this);
  detectorIndexes.push_back(newIndex); // We track the thing we just added.
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
