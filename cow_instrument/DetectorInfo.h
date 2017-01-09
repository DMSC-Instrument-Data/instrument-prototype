#ifndef DETECTOR_INFO_H
#define DETECTOR_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>

#include "ComponentProxy.h"
#include "cow_ptr.h"
#include "Detector.h"
#include "IdType.h"
#include "L1s.h"
#include "L2s.h"
#include "MaskFlags.h"
#include "MonitorFlags.h"
#include "Path.h"
#include "PathComponent.h"
#include "PathComponentInfo.h"
#include "PathFactory.h"
#include "ScanTime.h"
#include "Spectrum.h"
#include "SourceSampleDetectorPathFactory.h"

/**
 * DetectorInfo type. Provides Meta-data context to an InstrumentTree
 * of detectors, and a facade for modifications.
 * Meta-data is provided per detector.
 */
template <typename InstTree> class DetectorInfo {
public:
  template <typename InstSptrType, typename PathFactoryType>
  explicit DetectorInfo(InstSptrType &&instrumentTree,
                        PathFactoryType &&pathFactory,
                        ScanTime scanTime = ScanTime{});

  explicit DetectorInfo(std::shared_ptr<InstTree> &instrumentTree,
                        ScanTime scanTime = ScanTime{});

  explicit DetectorInfo(std::shared_ptr<InstTree> &&instrumentTree,
                        ScanTime scanTime = ScanTime{});

  template <typename InstSptrType, typename TimeIndexesType,
            typename ScanTimesType, typename PositionsType,
            typename RotationsType>
  explicit DetectorInfo(InstSptrType &&instrumentTree,
                        TimeIndexesType &&timeIndexes,
                        ScanTimesType &&scanTimes, PositionsType &&positions,
                        RotationsType &&rotations);

  void setMasked(size_t detectorIndex);

  bool isMasked(size_t detectorIndex) const;

  void setMonitor(size_t detectorIndex);

  bool isMonitor(size_t detectorIndex) const;

  double l2(size_t detectorIndex) const;

  double l2(size_t detectorIndex, size_t timeIndex) const;

  Eigen::Vector3d position(size_t detectorIndex) const;

  Eigen::Vector3d position(size_t detectorIndex, size_t timeIndex) const;

  Eigen::Quaterniond rotation(size_t detectorIndex) const;

  Eigen::Quaterniond rotation(size_t detectorIndex, size_t timeIndex) const;

  double l1(size_t detectorIndex) const;

  size_t detectorSize() const;

  const InstTree &const_instrumentTree() const;

  void moveDetector(size_t detectorIndex, const Eigen::Vector3d &offset);

  void moveDetector(size_t detectorIndex, size_t timeIndex,
                    const Eigen::Vector3d &offset);

  void moveDetectors(const std::vector<size_t> &detectorIndexes,
                     const Eigen::Vector3d &offset);

  void rotateDetector(size_t detectorIndex, const Eigen::Vector3d &axis,
                      const double &theta, const Eigen::Vector3d &center);

  void rotateDetectors(const std::vector<size_t> &detectorIndexes,
                       const Eigen::Vector3d &axis, const double &theta,
                       const Eigen::Vector3d &center);

  void rotatePathComponents(const std::vector<size_t> &pathComponentIndexes,
                            const Eigen::Vector3d &axis, const double &theta,
                            const Eigen::Vector3d &center);

  void movePathComponents(const std::vector<size_t> &pathComponentIndexes,
                          const Eigen::Vector3d &offset);

  std::vector<Spectrum> makeSpectra() const;

  const PathComponentInfo<InstTree> &pathComponentInfo() const;

  CowPtr<L2s> l2s() const;

  bool isScanning() const;

private:
  void init();
  void initL2();
  void initL1();

  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked;
  CowPtr<MonitorFlags> m_isMonitor;

  CowPtr<L1s> m_l1;
  CowPtr<L2s> m_l2;
  CowPtr<const Paths> m_l2Paths;
  CowPtr<const Paths> m_l1Paths;
  std::shared_ptr<const std::vector<size_t>> m_detectorComponentIndexes;
  /// Locally (detector) indexed positions
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// Locally (detector) indexed rotations
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
  /// Linear index map (detector indexed)
  std::shared_ptr<const std::vector<std::vector<size_t>>> m_linearIndexMap;
  /// Scan durations
  std::shared_ptr<const ScanTimes> m_durations;
  /// Path component information
  PathComponentInfo<InstTree> m_pathComponentInfo;
  /// Is scanning
  const bool m_isScanning = false;
};

namespace {

template <typename U>
void detectorRangeCheck(size_t detectorIndex, const U &container) {
  if (detectorIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "Detector at index " << detectorIndex << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}

double distance(const Eigen::Vector3d &a, const Eigen::Vector3d &b) {
  return (a - b).norm();
}

template <typename InstSptrType>
std::shared_ptr<std::vector<std::vector<size_t>>>
makeDefaultIndexes(InstSptrType &instrumentTree) {
  size_t size = instrumentTree->nDetectors();
  auto indexes = std::make_shared<std::vector<std::vector<size_t>>>(size);
  for (size_t i = 0; i < size; ++i) {
    // Always one time index per detector. Just need the detector index.
    (*indexes)[i] = std::vector<size_t>(1, i);
  }
  return indexes;
}
}

template <typename InstTree>
template <typename InstSptrType, typename PathFactoryType>
DetectorInfo<InstTree>::DetectorInfo(InstSptrType &&instrumentTree,
                                     PathFactoryType &&pathFactory,
                                     ScanTime scanTime)
    : m_l2Paths(pathFactory.createL2(*instrumentTree)),
      m_l1Paths(pathFactory.createL1(*instrumentTree)),
      m_nDetectors(instrumentTree->nDetectors()),
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors)),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_detectorComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->detectorComponentIndexes())),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(m_nDetectors)),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nDetectors)),
      m_linearIndexMap(makeDefaultIndexes(instrumentTree)),
      m_durations(std::make_shared<const ScanTimes>(1, scanTime)),
      m_pathComponentInfo(std::forward<InstSptrType>(instrumentTree)) {

  init();
}

template <typename InstTree>
DetectorInfo<InstTree>::DetectorInfo(std::shared_ptr<InstTree> &instrumentTree,
                                     ScanTime scanTime)
    : m_l2Paths(SourceSampleDetectorPathFactory<InstTree>{}.createL2(
          *instrumentTree)),
      m_l1Paths(SourceSampleDetectorPathFactory<InstTree>{}.createL1(
          *instrumentTree)),
      m_nDetectors(instrumentTree->nDetectors()),
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors)),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_detectorComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->detectorComponentIndexes())),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(m_nDetectors)),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nDetectors)),
      m_linearIndexMap(makeDefaultIndexes(instrumentTree)),
      m_durations(std::make_shared<const ScanTimes>(1, scanTime)),
      m_pathComponentInfo(instrumentTree) {

  init();
}

template <typename InstTree>
DetectorInfo<InstTree>::DetectorInfo(std::shared_ptr<InstTree> &&instrumentTree,
                                     ScanTime scanTime)
    : m_l2Paths(SourceSampleDetectorPathFactory<InstTree>{}.createL2(
          *instrumentTree)),
      m_l1Paths(SourceSampleDetectorPathFactory<InstTree>{}.createL1(
          *instrumentTree)),
      m_nDetectors(instrumentTree->nDetectors()),
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors)),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_detectorComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->detectorComponentIndexes())),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(m_nDetectors)),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nDetectors)),
      m_linearIndexMap(makeDefaultIndexes(instrumentTree)),
      m_durations(std::make_shared<const ScanTimes>(1, scanTime)),
      m_pathComponentInfo(
          std::forward<std::shared_ptr<InstTree>>(instrumentTree)) {

  init();
}

template <typename InstTree>
template <typename InstSptrType, typename TimeIndexesType,
          typename ScanTimesType, typename PositionsType,
          typename RotationsType>
DetectorInfo<InstTree>::DetectorInfo(InstSptrType &&instrumentTree,
                                     TimeIndexesType &&timeIndexes,
                                     ScanTimesType &&scanTimes,
                                     PositionsType &&positions,
                                     RotationsType &&rotations)
    : m_l2Paths(SourceSampleDetectorPathFactory<InstTree>{}.createL2(
          *instrumentTree)),
      m_l1Paths(SourceSampleDetectorPathFactory<InstTree>{}.createL1(
          *instrumentTree)),
      m_nDetectors(instrumentTree->nDetectors()),
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors * scanTimes.size())),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_detectorComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->detectorComponentIndexes())),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          std::forward<PositionsType>(positions))),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          std::forward<RotationsType>(rotations))),
      m_linearIndexMap(std::make_shared<std::vector<std::vector<size_t>>>(
          std::forward<TimeIndexesType>(timeIndexes))),
      m_durations(
          std::make_shared<ScanTimes>(std::forward<ScanTimesType>(scanTimes))),
      m_pathComponentInfo(
          std::forward<std::shared_ptr<InstTree>>(instrumentTree)),
      m_isScanning(true) {
  size_t indexEntryCount = 0;
  for (auto &positionTimes : *m_linearIndexMap) {
    // Count up the time indexes for each component
    indexEntryCount += positionTimes.size();
  }
  if (indexEntryCount != m_positions->size()) {
    throw std::invalid_argument("The number of component/time indexes does not "
                                "match the number of positions for scanning "
                                "construction");
  }
  if (indexEntryCount != m_rotations->size()) {
    throw std::invalid_argument("The number of component/time indexes does not "
                                "match the number of rotations for scanning "
                                "construction");
  }
  if ((m_positions->size() / m_linearIndexMap->size()) != m_durations->size()) {
    throw std::invalid_argument("Wrong number of scans");
  }

  initL1();
  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::setMasked(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  (*m_isMasked)[detectorIndex] = true;
}

template <typename InstTree> void DetectorInfo<InstTree>::init() {

  // TODO. Refactor this so that a copy is not required!
  std::vector<Eigen::Vector3d> allComponentPositions =
      m_pathComponentInfo.const_instrumentTree().startPositions();
  std::vector<Eigen::Quaterniond> allComponentRotations =
      m_pathComponentInfo.const_instrumentTree().startRotations();

  size_t i = 0;
  for (auto &compIndex : (*m_detectorComponentIndexes)) {
    (*m_positions)[i] = allComponentPositions[compIndex];
    (*m_rotations)[i] = allComponentRotations[compIndex];
    ++i;
  }

  initL1();
  initL2();
}

template <typename InstTree> void DetectorInfo<InstTree>::initL1() {

  /*
   * Caution for future extension of this: We must not double count the
   * length() of the Source PathComponent, as this Component will appear in both
   * the L1 and L2 calculation. Implementations of PathComponent, where
   * isSource() is set to true, should therefore provide the scattering
   * internal length as length/2.
  */

  const std::vector<Eigen::Vector3d> &entryPoints =
      m_pathComponentInfo.const_entryPoints();
  const std::vector<Eigen::Vector3d> &exitPoints =
      m_pathComponentInfo.const_exitPoints();
  const std::vector<double> &pathLengths =
      m_pathComponentInfo.const_pathLengths();

  // Loop over all detector indexes. We will have a path for each.
  for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
       ++detectorIndex) {

    size_t i = 0;
    const Path &path = (*m_l1Paths)[detectorIndex];
    if (path.size() < 2) {
      throw std::logic_error("Cannot have a L1 specified with less than 2 path "
                             "components (sample + source).");
    }

    double l1 = pathLengths[path[i]];
    for (i = 1; i < path.size(); ++i) {

      l1 += distance(entryPoints[path[i]], exitPoints[path[i - 1]]);
      l1 += pathLengths[path[i]];
    }

    (*m_l1)[detectorIndex] = l1;
  }
}

template <typename InstTree> void DetectorInfo<InstTree>::initL2() {

  const std::vector<Eigen::Vector3d> &entryPoints =
      m_pathComponentInfo.const_entryPoints();
  const std::vector<Eigen::Vector3d> &exitPoints =
      m_pathComponentInfo.const_exitPoints();
  const std::vector<double> &pathLengths =
      m_pathComponentInfo.const_pathLengths();

  const size_t scanCount = m_durations->size();

  // Loop over all detector indexes. We will have a path for each.
  for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
       ++detectorIndex) {

    for (size_t timeIndex = 0; timeIndex < scanCount; ++timeIndex) {

      auto detectorPos =
          (*m_positions)[(*m_linearIndexMap)[detectorIndex][timeIndex]];
      size_t i = 0;
      const Path &path = (*m_l2Paths)[detectorIndex];
      if (path.size() < 1) {
        throw std::logic_error(
            "Cannot have a L2 specified with less than 1 path "
            "components (sample).");
      }
      double l2 = pathLengths[path[i]];

      // For each detector-l2-path calculate the total neutronic length
      for (i = 1; i < path.size(); ++i) {
        l2 += distance(entryPoints[path[i]], exitPoints[path[i - 1]]);
        l2 += pathLengths[path[i]];
      }
      l2 += distance(exitPoints[path[i - 1]], detectorPos);

      (*m_l2)[(*m_linearIndexMap)[detectorIndex][timeIndex]] = l2;
    }
  }
}

template <typename InstTree>
bool DetectorInfo<InstTree>::isMasked(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  return m_isMasked.const_ref()[detectorIndex];
}

template <typename InstTree>
void DetectorInfo<InstTree>::setMonitor(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMonitor.const_ref());
  (*m_isMonitor)[detectorIndex] = true;
}

template <typename InstTree>
bool DetectorInfo<InstTree>::isMonitor(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_isMonitor.const_ref());
  return m_isMonitor.const_ref()[detectorIndex];
}

template <typename InstTree>
double DetectorInfo<InstTree>::l2(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_l2.const_ref());
  return m_l2.const_ref()[detectorIndex];
}

template <typename InstTree>
double DetectorInfo<InstTree>::l2(size_t detectorIndex,
                                  size_t timeIndex) const {
  detectorRangeCheck(detectorIndex, m_l2.const_ref());
  return m_l2.const_ref()[(*m_linearIndexMap)[detectorIndex][timeIndex]];
}

template <typename InstTree>
Eigen::Vector3d DetectorInfo<InstTree>::position(size_t detectorIndex) const {

  return (*m_positions)[detectorIndex];
}

template <typename InstTree>
Eigen::Vector3d DetectorInfo<InstTree>::position(size_t detectorIndex,
                                                 size_t timeIndex) const {

  return (*m_positions)[(*m_linearIndexMap)[detectorIndex][timeIndex]];
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotation(size_t detectorIndex) const {
  return (*m_rotations)[detectorIndex];
}

template <typename InstTree>
Eigen::Quaterniond DetectorInfo<InstTree>::rotation(size_t detectorIndex,
                                                    size_t timeIndex) const {
  return (*m_rotations)[(*m_linearIndexMap)[detectorIndex][timeIndex]];
}

template <typename InstTree>
double DetectorInfo<InstTree>::l1(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_l1.const_ref());
  return m_l1.const_ref()[detectorIndex];
}

template <typename InstTree>
size_t DetectorInfo<InstTree>::detectorSize() const {
  return m_nDetectors;
}

template <typename InstTree>
const InstTree &DetectorInfo<InstTree>::const_instrumentTree() const {
  return m_pathComponentInfo.const_instrumentTree();
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetector(size_t detectorIndex,
                                          const Eigen::Vector3d &offset) {

  (*m_positions)[detectorIndex] += offset;

  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetector(size_t detectorIndex,
                                          size_t timeIndex,
                                          const Eigen::Vector3d &offset) {

  (*m_positions)[(*m_linearIndexMap)[detectorIndex][timeIndex]] += offset;

  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetectors(
    const std::vector<size_t> &detectorIndexes, const Eigen::Vector3d &offset) {

  for (auto &detIndex : detectorIndexes) {
    (*m_positions)[detIndex] += offset;
  }

  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotateDetector(size_t detectorIndex,
                                            const Eigen::Vector3d &axis,
                                            const double &theta,
                                            const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();

  (*m_positions)[detectorIndex] = transform * (*m_positions)[detectorIndex];
  (*m_rotations)[detectorIndex] = rotation * (*m_rotations)[detectorIndex];

  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotateDetectors(
    const std::vector<size_t> &detectorIndexes, const Eigen::Vector3d &axis,
    const double &theta, const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();
  for (auto &detIndex : detectorIndexes) {

    (*m_positions)[detIndex] = transform * (*m_positions)[detIndex];
    (*m_rotations)[detIndex] = rotation * (*m_rotations)[detIndex];
  }

  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotatePathComponents(
    const std::vector<size_t> &pathComponentIndexes,
    const Eigen::Vector3d &axis, const double &theta,
    const Eigen::Vector3d &center) {

  m_pathComponentInfo.rotatePathComponents(pathComponentIndexes, axis, theta,
                                           center);

  initL1();
  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::movePathComponents(
    const std::vector<size_t> &pathComponentIndexes,
    const Eigen::Vector3d &offset) {

  m_pathComponentInfo.movePathComponents(pathComponentIndexes, offset);

  initL1();
  initL2();
}

template <typename InstTree>
std::vector<Spectrum> DetectorInfo<InstTree>::makeSpectra() const {
  std::vector<Spectrum> spectra;
  spectra.reserve(this->detectorSize());
  for (size_t i = 0; i < this->detectorSize(); ++i) {
    spectra.push_back(i);
  }
  return spectra;
}

template <typename InstTree> CowPtr<L2s> DetectorInfo<InstTree>::l2s() const {
  return m_l2;
}

template <typename InstTree>
const PathComponentInfo<InstTree> &
DetectorInfo<InstTree>::pathComponentInfo() const {
  return m_pathComponentInfo;
}

template <typename InstTree> bool DetectorInfo<InstTree>::isScanning() const {
  return m_isScanning;
}

#endif
