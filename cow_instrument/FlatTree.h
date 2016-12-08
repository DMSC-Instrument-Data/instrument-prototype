#ifndef FLATTREE_H
#define FLATTREE_H

#include <memory>
#include <vector>
#include <map>
#include <Eigen/Core>
#include "IdType.h"
#include "SOASource.h"

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
  /// Enable use to determine all sub-components.
  std::vector<size_t> subTreeIndexes(size_t proxyIndex) const;

  std::vector<Eigen::Vector3d> startPositions() const;
  std::vector<Eigen::Quaterniond> startRotations() const;
  std::vector<Eigen::Vector3d> startExitPoints() const;
  std::vector<Eigen::Vector3d> startEntryPoints() const;
  std::vector<double> pathLengths() const;
  std::vector<size_t> detectorComponentIndexes() const;

  size_t detIndexToCompIndex(size_t detectorIndex) const;
  size_t pathIndexToCompIndex(size_t pathIndex) const;

  // Needed for serialization.
  std::shared_ptr<Component> rootComponent() const;

private:
  void init();

  /// Path index
  size_t m_sourceIndex;
  /// Path index
  size_t m_sampleIndex;
  /// vector of proxies and relevant pointers
  SOASource m_source;
  /// Component root. Stashed. We only need this for serialization at present.
  std::shared_ptr<Component> m_componentRoot;
};

using FlatTree_const_uptr = std::unique_ptr<const FlatTree>;
using FlatTree_uptr = std::unique_ptr<const FlatTree>;

#endif
