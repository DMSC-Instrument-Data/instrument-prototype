#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>
#include <vector>
#include <map>
#include <Eigen/Core>
#include "cow_ptr.h"
#include "IdType.h"
#include "ComponentInfo.h"

class Detector;
class PathComponent;
class PointSource;
class PointSample;

/*
 The instrument is nothing more than syntatic sugar over the root Node.
 */
class InstrumentTree {
public:
  InstrumentTree(std::shared_ptr<Component> componentRoot);

  const ComponentProxy &rootProxy() const;

  const Detector &getDetector(size_t detectorIndex) const;

  const PathComponent &getPathComponent(size_t pathComponentIndex) const;
  const PathComponent &source() const;
  const PathComponent &sample() const;

  // This is how we help the client out when they want to work with detector
  // ids.
  void fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const;

  size_t nDetectors() const;
  size_t nPathComponents() const;

  const ComponentProxy &proxyAt(size_t index) const;

  size_t samplePathIndex() const;
  size_t sourcePathIndex() const;

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

  size_t detIndexToCompIndex(size_t detectorIndex) const;
  size_t pathIndexToCompIndex(size_t pathIndex) const;

  // Unsafe. Consider refactoring.
  std::shared_ptr<Component> rootComponent() const;

private:
  void init();
  /// PathComponent vector index of the source
  size_t m_sourceIndex;
  /// Pa_componentthComponent vector index of the sample
  size_t m_sampleIndex;
  /// vector of proxies and relevant pointers
  ComponentInfo m_componentInfo;
  /// Component root
  std::shared_ptr<Component>
      m_componentRoot; // TODO. This probably needs to be COW.
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;
using InstrumentTree_uptr = std::unique_ptr<const InstrumentTree>;

#endif
