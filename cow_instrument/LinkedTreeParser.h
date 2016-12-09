#ifndef LinkedTreeParser_H
#define LinkedTreeParser_H

#include <vector>
#include <cstddef>
#include "IdType.h"
#include "ComponentProxy.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <map>

class Detector;
class PathComponent;
class CompositeComponent;

/**
 * Converts a component tree doubly linked-list representation of an instrument
 * into a series of arrays and a flattened component-proxy representation.
 */
class LinkedTreeParser {
public:
  LinkedTreeParser() = default;
  void registerDetector(Detector const *const comp);
  void registerPathComponent(PathComponent const *const comp);
  size_t registerComposite(CompositeComponent const *const comp);
  void registerDetector(Detector const *const comp, size_t parentIndex);
  void registerPathComponent(PathComponent const *const comp,
                             size_t parentIndex);
  size_t registerComposite(CompositeComponent const *const comp,
                           size_t parentIndex);

  std::vector<ComponentProxy> proxies();
  size_t componentSize() const;
  size_t detectorSize() const;
  size_t pathSize() const;
  std::vector<size_t> pathComponentIndexes() const;
  std::vector<size_t> detectorComponentIndexes() const;
  std::vector<Eigen::Vector3d> startEntryPoints() const;
  std::vector<Eigen::Vector3d> startExitPoints() const;
  std::vector<double> pathLengths() const;
  std::vector<Eigen::Vector3d> startPositions() const;
  std::vector<Eigen::Quaterniond> startRotations() const;
  std::vector<ComponentIdType> componentIds() const;
  std::vector<DetectorIdType> detectorIds() const;
  int64_t sourcePathIndex() const;
  int64_t samplePathIndex() const;

private:
  size_t coreUpdate(Component const *const comp);
  size_t coreUpdate(Component const *const comp, size_t previousIndex);

  /// PathComponent vector index of the source
  int64_t m_sourceIndex = -1;
  /// PathComponent vector index of the sample
  int64_t m_sampleIndex = -1;

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
};

#endif
