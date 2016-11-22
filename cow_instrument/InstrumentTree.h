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

  // This is how we help the client out when they want to work with detector
  // ids.
  void fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const;

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
  ComponentInfo m_componentInfo;
  /// Component root
  std::shared_ptr<Component> m_componentRoot;
};

// Do not encourage use of. A convenience for testing.
namespace nonstandard {
/// Get the source Component. Should not be required!
const PathComponent &source(const InstrumentTree &instrumentTree);
/// Get the sample Component. Should not be required!
const PathComponent &sample(const InstrumentTree &instrumentTree);
/// Get the path component
const PathComponent &getPathComponent(const InstrumentTree &instrumentTree,
                                      size_t pathComponentIndex);
/// Get the detector
const Detector &getDetector(const InstrumentTree &instrumentTree,
                            size_t detectorIndex);
}

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;
using InstrumentTree_uptr = std::unique_ptr<const InstrumentTree>;

#endif
