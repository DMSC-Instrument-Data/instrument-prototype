#ifndef FLATTREE_H
#define FLATTREE_H

#include <memory>
#include <vector>
#include <map>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "IdType.h"

class Component;
class ComponentProxy;
class Detector;
class PathComponent;
class PointSource;
class PointSample;

/*
 Flattened representation of an Instrument Tree.
 */
class FlatTree {
public:
  FlatTree(std::shared_ptr<Component> componentRoot);
  /// A construction mechnanism that bypasses the old tree linked-list
  /// constructional approach
  FlatTree(std::vector<ComponentProxy> &&proxies,
           std::vector<Eigen::Vector3d> &&positions,
           std::vector<Eigen::Quaterniond> &&rotations,
           std::vector<ComponentIdType> &&componentIds,
           std::vector<Eigen::Vector3d> &&entryPoints,
           std::vector<Eigen::Vector3d> &&exitPoints,
           std::vector<double> &&pathLengths,
           std::vector<size_t> &&pathComponentIndexes,
           std::vector<size_t> &&detectorComponentIndexes,
           std::vector<DetectorIdType> &&detectorIds, size_t sourceIndex,
           size_t sampleIndex);

  const ComponentProxy &rootProxy() const;

  void fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const;
  void fillComponentMap(std::map<ComponentIdType, size_t> &toFill) const;

  size_t nDetectors() const;
  size_t nPathComponents() const;

  const ComponentProxy &proxyAt(size_t index) const;

  size_t samplePathIndex() const;
  size_t sourcePathIndex() const;
  size_t sampleComponentIndex() const;
  size_t sourceComponentIndex() const;

  std::vector<ComponentProxy>::const_iterator begin() const;
  std::vector<ComponentProxy>::const_iterator end() const;
  std::vector<ComponentProxy>::const_iterator cbegin() const;
  std::vector<ComponentProxy>::const_iterator cend() const;

  size_t componentSize() const;
  /// Enable use to determine all sub-components proxy indexes.
  std::vector<size_t> subTreeIndexes(size_t proxyIndex) const;
  /// Enable use to determine sub-component proxy indexes only down to the next
  /// level.
  std::vector<size_t> nextLevelIndexes(size_t proxyIndex) const;

  std::vector<Eigen::Vector3d> startPositions() const;
  std::vector<Eigen::Quaterniond> startRotations() const;
  std::vector<Eigen::Vector3d> startExitPoints() const;
  std::vector<Eigen::Vector3d> startEntryPoints() const;
  std::vector<double> pathLengths() const;
  std::vector<size_t> detectorComponentIndexes() const;
  std::vector<size_t> pathComponentIndexes() const;

  size_t detIndexToCompIndex(size_t detectorIndex) const;
  size_t pathIndexToCompIndex(size_t pathIndex) const;

  // Needed for serialization.
  std::shared_ptr<Component> rootComponent() const;

  bool operator==(const FlatTree &other) const;
  bool operator!=(const FlatTree &other) const;

private:

  /// Path index
  size_t m_sourceIndex;
  /// Path index
  size_t m_sampleIndex;

  /*
   These collections have the same size as the number of components. They are
   component
   type independent
   */
  std::vector<ComponentProxy> m_proxies;
  std::vector<Eigen::Vector3d> m_positions;
  std::vector<Eigen::Quaterniond> m_rotations;
  std::vector<ComponentIdType> m_componentIds;

  /*
    These collections are conditionally updated depending upon component type.
    The vector
    of indexes allows us to go from say detector_index -> component_index.
   */
  std::vector<Eigen::Vector3d> m_entryPoints; // For path components
  std::vector<Eigen::Vector3d> m_exitPoints;  // For path components
  std::vector<double> m_pathLengths;          // For path components
  std::vector<size_t> m_pathComponentIndexes;
  std::vector<size_t> m_detectorComponentIndexes;
  std::vector<DetectorIdType> m_detectorIds;
  std::shared_ptr<Component> m_componentRoot;
};

using FlatTree_const_uptr = std::unique_ptr<const FlatTree>;
using FlatTree_uptr = std::unique_ptr<const FlatTree>;

#endif
