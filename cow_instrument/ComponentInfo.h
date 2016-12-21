#ifndef COMPONENT_INFO_H
#define COMPONENT_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>

#include "ComponentProxy.h"
#include "cow_ptr.h"
#include "DetectorInfo.h"

#include "IdType.h"

/**
 * ComponentInfo type. Provides meta-data an behaviour for working with a
 * FlatTree
 * at the component level. Meta-data is provided per component. Add detectors
 * are components.
 */
template <typename InstTree> class ComponentInfo {
public:
  explicit ComponentInfo(std::shared_ptr<InstTree> &&instrumentTree);
  explicit ComponentInfo(std::shared_ptr<InstTree> &instrumentTree);

  explicit ComponentInfo(
      std::shared_ptr<DetectorInfo<InstTree>> &&detectorInfo);
  explicit ComponentInfo(std::shared_ptr<DetectorInfo<InstTree>> &detectorInfo);

  Eigen::Vector3d position(size_t componentIndex) const;

  Eigen::Quaterniond rotation(size_t componentIndex) const;

  size_t size() const;

  size_t componentSize() const;

  const InstTree &const_instrumentTree() const;

  void move(size_t componentIndex, const Eigen::Vector3d &offset);

  void rotate(size_t componentIndex, const Eigen::Vector3d &axis,
              const double &theta, const Eigen::Vector3d &center);

  void move2(size_t componentIndex, const Eigen::Vector3d &offset);

  void rotate2(size_t componentIndex, const Eigen::Vector3d &axis,
               const double &theta, const Eigen::Vector3d &center);

private:
  /// Detector info
  std::shared_ptr<const DetectorInfo<InstTree>> m_detectorInfo;
  /// Instrument tree.
  std::shared_ptr<const InstTree> m_instrumentTree; // Legacy. Remove
  /// Instrument tree
  const InstTree &m_instrumentTree2;
  /// All positions indexed by component index. Owned by ComponentInfo.
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// All rotations indexed by component index. Owned by ComponentInfo.
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
  // TODO shapes and parameters would also be stored here
  std::vector<int64_t> m_componentToDetector;

  void makeInvertedMap();
};

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    std::shared_ptr<InstTree> &instrumentTree)
    : m_instrumentTree(instrumentTree), m_instrumentTree2(*m_instrumentTree),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree->startRotations()))

{}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    std::shared_ptr<InstTree> &&instrumentTree)
    : m_instrumentTree(std::forward<std::shared_ptr<InstTree>>(instrumentTree)),
      m_instrumentTree2(*m_instrumentTree),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree->startRotations()))

{}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    std::shared_ptr<DetectorInfo<InstTree>> &detectorInfo)
    : m_detectorInfo(detectorInfo),
      m_instrumentTree2(m_detectorInfo->const_instrumentTree()),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree2.startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree2.startRotations())),
      m_componentToDetector(m_instrumentTree2.componentSize(), -1) {

  makeInvertedMap();
}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    std::shared_ptr<DetectorInfo<InstTree>> &&detectorInfo)
    : m_detectorInfo(std::move(detectorInfo)),
      m_instrumentTree2(m_detectorInfo->const_instrumentTree()),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree2.startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree2.startRotations())),
      m_componentToDetector(m_instrumentTree2.componentSize(), -1) {
  makeInvertedMap();
}

template <typename InstTree> void ComponentInfo<InstTree>::makeInvertedMap() {

  // Invert the map.
  auto detToComponent = m_instrumentTree2.detectorComponentIndexes();
  for (size_t i = 0; i < detToComponent.size(); ++i) {
    m_componentToDetector[detToComponent[i]] = i;
  }
}

template <typename InstTree>
Eigen::Vector3d ComponentInfo<InstTree>::position(size_t componentIndex) const {
  return (*m_positions)[componentIndex];
}

template <typename InstTree>
Eigen::Quaterniond
ComponentInfo<InstTree>::rotation(size_t componentIndex) const {
  return (*m_rotations)[componentIndex];
}

template <typename InstTree>
size_t ComponentInfo<InstTree>::componentSize() const {
  return m_instrumentTree->componentSize();
}

template <typename InstTree>
const InstTree &ComponentInfo<InstTree>::const_instrumentTree() const {
  return *m_instrumentTree;
}

template <typename InstTree>
void ComponentInfo<InstTree>::move(size_t componentIndex,
                                   const Eigen::Vector3d &offset) {

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);
  for (auto &index : indexes) {
    (*m_positions)[index] += offset;
    // TODO m_entry and m_exit points should also be translated here!
  }
}

template <typename InstTree>
void ComponentInfo<InstTree>::move2(size_t componentIndex,
                                    const Eigen::Vector3d &offset) {

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);

  std::vector<size_t>
      detectorsToMove; // we would have an equivalent for path components.
  for (auto &compIndex : indexes) {
    auto detIndex = m_componentToDetector[compIndex];
    if (detIndex >= 0) {
      detectorsToMove.push_back(detIndex);
    }
    // TODO m_entry and m_exit points should also be translated here!
  }

  m_detectorInfo->moveDetectors(detectorsToMove, componentIndex, offset);
}

template <typename InstTree>
void ComponentInfo<InstTree>::rotate2(size_t componentIndex,
                                      const Eigen::Vector3d &axis,
                                      const double &theta,
                                      const Eigen::Vector3d &center) {

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);

  std::vector<size_t> detectorsToRotate;
  for (auto &compIndex : indexes) {
    auto detIndex = m_componentToDetector[compIndex];
    if (detIndex >= 0) {
      detectorsToRotate.push_back(detIndex);
    }
    // TODO m_entry and m_exit points should also be translated here!
  }
  m_detectorInfo->rotateDetectors(detectorsToRotate, axis, theta, center);
}

template <typename InstTree>
void ComponentInfo<InstTree>::rotate(size_t componentIndex,
                                     const Eigen::Vector3d &axis,
                                     const double &theta,
                                     const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);

  for (auto &index : indexes) {
    (*m_positions)[index] = transform * (*m_positions)[index];
    (*m_rotations)[index] = rotation * (*m_rotations)[index];
    // TODO m_entry and m_exit points should also be translated here!
  }
}

#endif
