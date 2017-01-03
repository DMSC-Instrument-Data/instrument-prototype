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

/**
 * ComponentInfo type. Provides meta-data an behaviour for working with a
 * FlatTree at the component level. Wraps DetectorInfo and PatchComponentInfo
 * and therefore
 * gives a Component only view of the data and associated operations.
 */
template <typename InstTree> class ComponentInfo {
public:
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

private:
  /// Detector info
  std::shared_ptr<DetectorInfo<InstTree>> m_detectorInfo;
  /// Instrument tree
  const InstTree &m_instrumentTree;
  /// Inverted map to get detector indexes from component indexes
  std::vector<int64_t> m_componentToDetectorIndex;
  /// Inverted map to get path component indexes indexes from component indexes
  std::vector<int64_t> m_componentToPathIndex;
  void makeInvertedMaps();
};

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    std::shared_ptr<DetectorInfo<InstTree>> &detectorInfo)
    : m_detectorInfo(detectorInfo),
      m_instrumentTree(m_detectorInfo->const_instrumentTree()),
      m_componentToDetectorIndex(m_instrumentTree.componentSize(), -1),
      m_componentToPathIndex(m_instrumentTree.componentSize(), -1) {

  makeInvertedMaps();
}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    std::shared_ptr<DetectorInfo<InstTree>> &&detectorInfo)
    : m_detectorInfo(std::move(detectorInfo)),
      m_instrumentTree(m_detectorInfo->const_instrumentTree()),
      m_componentToDetectorIndex(m_instrumentTree.componentSize(), -1),
      m_componentToPathIndex(m_instrumentTree.componentSize(), -1) {
  makeInvertedMaps();
}

template <typename InstTree> void ComponentInfo<InstTree>::makeInvertedMaps() {

  // Invert the map.
  auto detToComponent = m_instrumentTree.detectorComponentIndexes();
  auto pathToComponent = m_instrumentTree.pathComponentIndexes();
  for (size_t i = 0; i < detToComponent.size(); ++i) {
    m_componentToDetectorIndex[detToComponent[i]] = i;
  }
  for (size_t i = 0; i < pathToComponent.size(); ++i) {
    m_componentToPathIndex[pathToComponent[i]] = i;
  }
}


template <typename InstTree>
Eigen::Vector3d ComponentInfo<InstTree>::position(size_t componentIndex) const {

  const std::vector<size_t> componentIndexes =
      m_instrumentTree.nextLevelIndexes(componentIndex);
  const auto nextLevelSize = componentIndexes.size();
  if (nextLevelSize > 0) {
    // We are dealing with a composite
    auto pos = Eigen::Vector3d{0, 0, 0};
    for (auto &index : componentIndexes) {
      pos += this->position(index);
    }
    pos /= nextLevelSize;
    return pos;
  } else {
    // Is either a path component or a detector
    auto detIndex = m_componentToDetectorIndex[componentIndex];
    if (detIndex >= 0) {
      return m_detectorInfo->position(detIndex);
    } else {
      auto pathIndex = m_componentToPathIndex[componentIndex];

      return m_detectorInfo->pathComponentInfo().position(pathIndex);
    }
  }
}

template <typename InstTree>
Eigen::Quaterniond
ComponentInfo<InstTree>::rotation(size_t componentIndex) const {
  const std::vector<size_t> componentIndexes =
      m_instrumentTree.nextLevelIndexes(componentIndex);
  const auto nextLevelSize = componentIndexes.size();
  if (nextLevelSize > 0) {
    // We are dealing with a composite
    throw std::runtime_error("Rotations for composites not implemented");
  } else {
    // Is either a path component or a detector
    auto detIndex = m_componentToDetectorIndex[componentIndex];
    if (detIndex >= 0) {
      return m_detectorInfo->rotation(detIndex);
    } else {
      auto pathIndex = m_componentToPathIndex[componentIndex];

      return m_detectorInfo->pathComponentInfo().rotation(pathIndex);
    }
  }
}



template <typename InstTree>
size_t ComponentInfo<InstTree>::componentSize() const {
  return m_instrumentTree.componentSize();
}

template <typename InstTree>
const InstTree &ComponentInfo<InstTree>::const_instrumentTree() const {
  return m_instrumentTree;
}

template <typename InstTree>
void ComponentInfo<InstTree>::move(size_t componentIndex,
                                   const Eigen::Vector3d &offset) {

  // All "connected" component indexes
  const std::vector<size_t> componentIndexes =
      m_instrumentTree.subTreeIndexes(componentIndex);

  std::vector<size_t>
      detectorsToMove; // Cache of detector indexes which will be moved.
  std::vector<size_t> pathComponentsToMove; // Cache of path component indexes
                                            // which will be moved
  for (auto &compIndex : componentIndexes) {
    auto detIndex = m_componentToDetectorIndex[compIndex];
    if (detIndex >= 0) {
      detectorsToMove.push_back(detIndex);
    }

    auto pathIndex = m_componentToPathIndex[compIndex];
    if (pathIndex >= 0) {
      pathComponentsToMove.push_back(pathIndex);
    }

    // Anything else in the subtree is ignored. Correctly.
  }

  m_detectorInfo->moveDetectors(detectorsToMove, offset);
  m_detectorInfo->movePathComponents(pathComponentsToMove, offset);
}

template <typename InstTree>
void ComponentInfo<InstTree>::rotate(size_t componentIndex,
                                     const Eigen::Vector3d &axis,
                                     const double &theta,
                                     const Eigen::Vector3d &center) {

  // All "connected" component indexes
  const std::vector<size_t> componentIndexes =
      m_instrumentTree.subTreeIndexes(componentIndex);

  std::vector<size_t>
      detectorsToRotate; // Cache of detector indexes which will be rotated.
  std::vector<size_t> pathComponentsToRotate; // Cache of path component indexes
                                              // which will be rotated
  for (auto &compIndex : componentIndexes) {
    auto detIndex = m_componentToDetectorIndex[compIndex];
    if (detIndex >= 0) {
      detectorsToRotate.push_back(detIndex);
    }

    auto pathIndex = m_componentToPathIndex[compIndex];
    if (pathIndex >= 0) {
      pathComponentsToRotate.push_back(pathIndex);
    }

    // Anything else in the subtree is ignored. Correctly.
  }

  m_detectorInfo->rotateDetectors(detectorsToRotate, axis, theta, center);
  m_detectorInfo->rotatePathComponents(pathComponentsToRotate, axis, theta,
                                       center);
}

#endif
