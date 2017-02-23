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
#include "AssemblyInfo.h"
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
  /// Assembly info (branched nodes)
  AssemblyInfo<InstTree> m_assemblyInfo;
  /// Instrument tree
  const InstTree &m_instrumentTree;
  /// Inverted map to get detector indexes from component indexes
  std::vector<int64_t> m_componentToDetectorIndex;
  /// Inverted map to get path component indexes  from component indexes
  std::vector<int64_t> m_componentToPathIndex;
  /// Inverted map to get branch node indexes from component indexes
  std::vector<int64_t> m_componentToBranchNodeIndex;
};

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    const DetectorInfo<InstTree> &detectorInfo)
    : m_detectorInfo(detectorInfo),
      m_assemblyInfo(detectorInfo.const_instrumentTree()),
      m_instrumentTree(m_detectorInfo.const_instrumentTree()),
      m_componentToDetectorIndex(m_instrumentTree.componentSize(), -1),
      m_componentToPathIndex(m_instrumentTree.componentSize(), -1),
      m_componentToBranchNodeIndex(m_instrumentTree.componentSize(), -1)

{

  init();
}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    const DetectorInfo<InstTree> &&detectorInfo)
    : m_detectorInfo(std::move(detectorInfo)),
      m_assemblyInfo(m_detectorInfo.const_instrumentTree()),
      m_instrumentTree(m_detectorInfo.const_instrumentTree()),
      m_componentToDetectorIndex(m_instrumentTree.componentSize(), -1),
      m_componentToPathIndex(m_instrumentTree.componentSize(), -1),
      m_componentToBranchNodeIndex(m_instrumentTree.componentSize(), -1)

{

  init();
}


template <typename InstrTree> void ComponentInfo<InstrTree>::init() {
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
  auto assemblyIndex = m_componentToBranchNodeIndex[componentIndex];
  return m_assemblyInfo.position(assemblyIndex);
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
  auto assemblyIndex = m_componentToBranchNodeIndex[componentIndex];
  return m_assemblyInfo.rotation(assemblyIndex);
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
  std::vector<size_t> assemblyComponentsToMove; // Cache of assembly component
                                                // indexes which will be moved.

  detectorsToMove.reserve(componentIndexes.size());
  pathComponentsToMove.reserve(componentIndexes.size());
  assemblyComponentsToMove.reserve(componentIndexes.size());
  for (auto &compIndex : componentIndexes) {
    auto detIndex = m_componentToDetectorIndex[compIndex];
    if (detIndex >= 0) {
      detectorsToMove.push_back(detIndex);
    }

    auto pathIndex = m_componentToPathIndex[compIndex];
    if (pathIndex >= 0) {
      pathComponentsToMove.push_back(pathIndex);
    }

    auto assemblyIndex = m_componentToBranchNodeIndex[compIndex];
    if (assemblyIndex >= 0) {
      /*
       * Note that sub-components of the assembly are handled at the
       * component-index level (see FlatTree::subTree).
       */
      assemblyComponentsToMove.push_back(assemblyIndex);
    }
  }

  m_assemblyInfo.moveAssemblyComponents(assemblyComponentsToMove, offset);
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
  std::vector<size_t>
      assemblyComponentsToRotate; // Cache of assembly indexes to rotate
  detectorsToRotate.reserve(componentIndexes.size());
  pathComponentsToRotate.reserve(componentIndexes.size());
  assemblyComponentsToRotate.reserve(componentIndexes.size());
  for (auto &compIndex : componentIndexes) {
    auto detIndex = m_componentToDetectorIndex[compIndex];
    if (detIndex >= 0) {
      detectorsToRotate.push_back(detIndex);
    }

    auto pathIndex = m_componentToPathIndex[compIndex];
    if (pathIndex >= 0) {
      pathComponentsToRotate.push_back(pathIndex);
    }

    auto assemblyIndex = m_componentToBranchNodeIndex[compIndex];
    if (assemblyIndex >= 0) {
      assemblyComponentsToRotate.push_back(assemblyIndex);
    }
  }

  m_assemblyInfo.rotateAssemblyComponents(assemblyComponentsToRotate, axis,
                                          theta, center);
  m_detectorInfo.rotateDetectors(detectorsToRotate, axis, theta, center);
  m_detectorInfo.rotatePathComponents(pathComponentsToRotate, axis, theta,
                                       center);
}

#endif
