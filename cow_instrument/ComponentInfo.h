#ifndef COMPONENT_INFO_H
#define COMPONENT_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>

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
  explicit ComponentInfo(const DetectorInfo<InstTree> &detectorInfo);
  explicit ComponentInfo(const DetectorInfo<InstTree> &&detectorInfo);

  Eigen::Vector3d position(size_t componentIndex) const;

  Eigen::Quaterniond rotation(size_t componentIndex) const;

  size_t size() const;

  size_t componentSize() const;

  const InstTree &const_instrumentTree() const;

  void move(size_t componentIndex, const Eigen::Vector3d &offset);

  void rotate(size_t componentIndex, const Eigen::Vector3d &axis,
              const double &theta, const Eigen::Vector3d &center);

private:
  /// initalization
  void init();
  /// Make maps with component index as key.
  void makeInvertedMaps();
  /// Detector info
  DetectorInfo<InstTree> m_detectorInfo;
  /// Instrument tree
  const InstTree &m_instrumentTree;
  /// Inverted map to get detector indexes from component indexes
  std::vector<int64_t> m_componentToDetectorIndex;
  /// Inverted map to get path component indexes  from component indexes
  std::vector<int64_t> m_componentToPathIndex;
  /// Inverted map to get branch node indexes from component indexes
  std::vector<int64_t> m_componentToBranchNodeIndex;
  /// Locally (branch node) indexed positions
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// Locally (branch node) indexed rotations
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
  /// branch node component indexes
  std::shared_ptr<const std::vector<size_t>> m_branchNodeComponentIndexes;
};

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(const DetectorInfo<InstTree> &detectorInfo)
    : m_detectorInfo(detectorInfo),
      m_instrumentTree(m_detectorInfo.const_instrumentTree()),
      m_componentToDetectorIndex(m_instrumentTree.componentSize(), -1),
      m_componentToPathIndex(m_instrumentTree.componentSize(), -1),
      m_componentToBranchNodeIndex(m_instrumentTree.componentSize(), -1),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree.nBranchNodeComponents())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree.nBranchNodeComponents())),
      m_branchNodeComponentIndexes(std::make_shared<std::vector<size_t>>(
          m_instrumentTree.branchNodeComponentIndexes()))

{

  init();
}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(const DetectorInfo<InstTree> &&detectorInfo)
    : m_detectorInfo(std::move(detectorInfo)),
      m_instrumentTree(m_detectorInfo.const_instrumentTree()),
      m_componentToDetectorIndex(m_instrumentTree.componentSize(), -1),
      m_componentToPathIndex(m_instrumentTree.componentSize(), -1),
      m_componentToBranchNodeIndex(m_instrumentTree.componentSize(), -1),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree.nBranchNodeComponents())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree.nBranchNodeComponents())),
      m_branchNodeComponentIndexes(std::make_shared<std::vector<size_t>>(
          m_instrumentTree.branchNodeComponentIndexes()))

{

  init();
}


template <typename InstrTree> void ComponentInfo<InstrTree>::init() {
  // TODO. Do this without copying everything!
  std::vector<Eigen::Vector3d> allComponentPositions =
      m_instrumentTree.startPositions();
  std::vector<Eigen::Quaterniond> allComponentRotations =
      m_instrumentTree.startRotations();

  size_t i = 0;
  for (auto &compIndex : (*m_branchNodeComponentIndexes)) {
    (*m_positions)[i] = allComponentPositions[compIndex];
    (*m_rotations)[i] = allComponentRotations[compIndex];
    ++i;
  }
  makeInvertedMaps();
}

template <typename InstTree> void ComponentInfo<InstTree>::makeInvertedMaps() {

  // Invert the map. Allows us to use component index as a key.
  auto detToComponent = m_instrumentTree.detectorComponentIndexes();
  auto pathToComponent = m_instrumentTree.pathComponentIndexes();
  auto branchNodeToComponent = m_instrumentTree.branchNodeComponentIndexes();
  for (size_t i = 0; i < detToComponent.size(); ++i) {
    m_componentToDetectorIndex[detToComponent[i]] = i;
  }
  for (size_t i = 0; i < pathToComponent.size(); ++i) {
    m_componentToPathIndex[pathToComponent[i]] = i;
  }
  for (size_t i = 0; i < branchNodeToComponent.size(); ++i) {
    m_componentToBranchNodeIndex[branchNodeToComponent[i]] = i;
  }
}

template <typename InstTree>
Eigen::Vector3d ComponentInfo<InstTree>::position(size_t componentIndex) const {

  auto detIndex = m_componentToDetectorIndex[componentIndex];
  if (detIndex >= 0) {
    return m_detectorInfo.position(detIndex);
  }

  auto pathIndex = m_componentToPathIndex[componentIndex];
  if (pathIndex >= 0) {
    return m_detectorInfo.pathComponentInfo().position(pathIndex);
  }
  auto branchNodeIndex = m_componentToBranchNodeIndex[componentIndex];
  return (*m_positions)[branchNodeIndex];
}

template <typename InstTree>
Eigen::Quaterniond
ComponentInfo<InstTree>::rotation(size_t componentIndex) const {
  auto detIndex = m_componentToDetectorIndex[componentIndex];
  if (detIndex >= 0) {
    return m_detectorInfo.rotation(detIndex);
  }

  auto pathIndex = m_componentToPathIndex[componentIndex];
  if (pathIndex >= 0) {
    return m_detectorInfo.pathComponentInfo().rotation(pathIndex);
  }
  auto branchNodeIndex = m_componentToBranchNodeIndex[componentIndex];
  return (*m_rotations)[branchNodeIndex];
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

  detectorsToMove.reserve(componentIndexes.size());
  pathComponentsToMove.reserve(componentIndexes.size());
  for (auto &compIndex : componentIndexes) {
    auto detIndex = m_componentToDetectorIndex[compIndex];
    if (detIndex >= 0) {
      detectorsToMove.push_back(detIndex);
    }

    auto pathIndex = m_componentToPathIndex[compIndex];
    if (pathIndex >= 0) {
      pathComponentsToMove.push_back(pathIndex);
    }

    auto branchNodeIndex = m_componentToBranchNodeIndex[compIndex];
    if (branchNodeIndex >= 0) {
      (*m_positions)[branchNodeIndex] += offset;
    }
  }

  m_detectorInfo.moveDetectors(detectorsToMove, offset);
  m_detectorInfo.movePathComponents(pathComponentsToMove, offset);
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

  detectorsToRotate.reserve(componentIndexes.size());
  pathComponentsToRotate.reserve(componentIndexes.size());
  for (auto &compIndex : componentIndexes) {
    auto detIndex = m_componentToDetectorIndex[compIndex];
    if (detIndex >= 0) {
      detectorsToRotate.push_back(detIndex);
    }

    auto pathIndex = m_componentToPathIndex[compIndex];
    if (pathIndex >= 0) {
      pathComponentsToRotate.push_back(pathIndex);
    }

    auto branchNodeIndex = m_componentToBranchNodeIndex[compIndex];
    if (branchNodeIndex >= 0) {
      using namespace Eigen;
      const auto transform = Translation3d(center) * AngleAxisd(theta, axis) *
                             Translation3d(-center);
      const auto rotation = transform.rotation();

      (*m_positions)[branchNodeIndex] =
          transform * (*m_positions)[branchNodeIndex];
      (*m_rotations)[branchNodeIndex] =
          rotation * (*m_rotations)[branchNodeIndex];
    }
  }

  m_detectorInfo.rotateDetectors(detectorsToRotate, axis, theta, center);
  m_detectorInfo.rotatePathComponents(pathComponentsToRotate, axis, theta,
                                       center);
}

#endif
