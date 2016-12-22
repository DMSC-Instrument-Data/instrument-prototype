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
                        PathFactoryType &&pathFactory);

  explicit DetectorInfo(std::shared_ptr<InstTree> &instrumentTree);

  explicit DetectorInfo(std::shared_ptr<InstTree> &&instrumentTree);

  void setMasked(size_t detectorIndex);

  bool isMasked(size_t detectorIndex) const;

  void setMonitor(size_t detectorIndex);

  bool isMonitor(size_t detectorIndex) const;

  double l2(size_t detectorIndex) const;

  Eigen::Vector3d positionDetector(size_t detectorIndex) const;

  Eigen::Quaterniond rotationDetector(size_t detectorIndex) const;

  double l1(size_t detectorIndex) const;

  size_t detectorSize() const;

  const InstTree &const_instrumentTree() const;

  void moveDetector(size_t detectorIndex, const Eigen::Vector3d &offset);

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

private:
  void init();
  void initL2(const std::vector<Eigen::Vector3d> &entryPoints,
              const std::vector<Eigen::Vector3d> &exitPoints,
              const std::vector<double> &pathLengths);
  void initL1(const std::vector<Eigen::Vector3d> &entryPoints,
              const std::vector<Eigen::Vector3d> &exitPoints,
              const std::vector<double> &pathLengths);

  //------------------- Metadata -------------
  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked;
  CowPtr<MonitorFlags> m_isMonitor;

  //------------------- DerivedInfo -------------
  CowPtr<L1s> m_l1;
  CowPtr<L2s> m_l2;
  CowPtr<const Paths> m_l2Paths;
  CowPtr<const Paths> m_l1Paths;
  std::shared_ptr<const std::vector<size_t>> m_detectorComponentIndexes;
  /// Component info
  std::shared_ptr<InstTree> m_instrumentTree;
  /// Locally (detector) indexed positions
  std::shared_ptr<std::vector<Eigen::Vector3d>> m_positions;
  /// Locally (detector) indexed rotations
  std::shared_ptr<std::vector<Eigen::Quaterniond>> m_rotations;
  /// Path component information
  CowPtr<PathComponentInfo<InstTree>> m_pathComponentInfo;
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
}

template <typename InstTree>
template <typename InstSptrType, typename PathFactoryType>
DetectorInfo<InstTree>::DetectorInfo(InstSptrType &&instrumentTree,
                                     PathFactoryType &&pathFactory)
    : m_l2Paths(pathFactory.createL2(*instrumentTree)),
      m_l1Paths(pathFactory.createL1(*instrumentTree)),
      m_nDetectors(instrumentTree->nDetectors()),
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors)),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_detectorComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->detectorComponentIndexes())),
      m_instrumentTree(std::forward<InstSptrType>(instrumentTree)),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(m_nDetectors)),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nDetectors)),
      m_pathComponentInfo(
          std::make_shared<PathComponentInfo<InstTree>>(m_instrumentTree)) {

  init();
}

template <typename InstTree>
DetectorInfo<InstTree>::DetectorInfo(std::shared_ptr<InstTree> &instrumentTree)
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
      m_instrumentTree(instrumentTree),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(m_nDetectors)),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nDetectors)),
      m_pathComponentInfo(
          std::make_shared<PathComponentInfo<InstTree>>(m_instrumentTree)) {

  init();
}

template <typename InstTree>
DetectorInfo<InstTree>::DetectorInfo(std::shared_ptr<InstTree> &&instrumentTree)
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
      m_instrumentTree(std::forward<std::shared_ptr<InstTree>>(instrumentTree)),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(m_nDetectors)),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nDetectors)),
      m_pathComponentInfo(
          std::make_shared<PathComponentInfo<InstTree>>(m_instrumentTree)) {

  init();
}

template <typename InstTree>
void DetectorInfo<InstTree>::setMasked(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  (*m_isMasked)[detectorIndex] = true;
}

template <typename InstTree> void DetectorInfo<InstTree>::init() {

  // TODO. Refactor this so that a copy is not required!
  std::vector<Eigen::Vector3d> allComponentPositions =
      m_instrumentTree->startPositions();
  std::vector<Eigen::Quaterniond> allComponentRotations =
      m_instrumentTree->startRotations();

  size_t i = 0;
  for (auto &compIndex : (*m_detectorComponentIndexes)) {
    (*m_positions)[i] = allComponentPositions[compIndex];
    (*m_rotations)[i] = allComponentRotations[compIndex];
    ++i;
  }

  /* Be warned that DetectorInfo is going to be internally indexing the
     following by path
     component index not detector index.
  */
  const std::vector<Eigen::Vector3d> &entryPoints =
      m_pathComponentInfo->const_entryPoints();
  const std::vector<Eigen::Vector3d> &exitPoints =
      m_pathComponentInfo->const_exitPoints();
  const std::vector<double> &pathLengths =
      m_pathComponentInfo->const_pathLengths();

  initL1(entryPoints, exitPoints, pathLengths);
  initL2(entryPoints, exitPoints, pathLengths);
}

template <typename InstTree>
void
DetectorInfo<InstTree>::initL1(const std::vector<Eigen::Vector3d> &entryPoints,
                               const std::vector<Eigen::Vector3d> &exitPoints,
                               const std::vector<double> &pathLengths) {

  /*
   * Caution for future extension of this: We must not double count the
   * length() of the Source PathComponent, as this Component will appear in both
   * the L1 and L2 calculation. Implementations of PathComponent, where
   * isSource() is set to true, should therefore provide the scattering
   * internal length as length/2.
  */

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

template <typename InstTree>
void
DetectorInfo<InstTree>::initL2(const std::vector<Eigen::Vector3d> &entryPoints,
                               const std::vector<Eigen::Vector3d> &exitPoints,
                               const std::vector<double> &pathLengths) {

  // Loop over all detector indexes. We will have a path for each.
  for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
       ++detectorIndex) {

    auto detectorPos = (*m_positions)[detectorIndex];
    size_t i = 0;
    const Path &path = (*m_l2Paths)[detectorIndex];
    if (path.size() < 1) {
      throw std::logic_error("Cannot have a L2 specified with less than 1 path "
                             "components (sample).");
    }
    double l2 = pathLengths[path[i]];

    // For each detector-l2-path calculate the total neutronic length
    for (i = 1; i < path.size(); ++i) {
      l2 += distance(entryPoints[path[i]], exitPoints[path[i - 1]]);
      l2 += pathLengths[path[i]];
    }
    l2 += distance(exitPoints[path[i - 1]], detectorPos);

    (*m_l2)[detectorIndex] = l2;
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
Eigen::Vector3d
DetectorInfo<InstTree>::positionDetector(size_t detectorIndex) const {

  return (*m_positions)[detectorIndex];
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotationDetector(size_t detectorIndex) const {
  return (*m_rotations)[detectorIndex];
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
  return *m_instrumentTree;
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetector(size_t detectorIndex,
                                          const Eigen::Vector3d &offset) {

  (*m_positions)[detectorIndex] += offset;

  // TODO should be L2 only
  init();
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetectors(
    const std::vector<size_t> &detectorIndexes, const Eigen::Vector3d &offset) {

  for (auto &detIndex : detectorIndexes) {
    (*m_positions)[detIndex] += offset;
  }

  // Only l2 needs to be recalculated. TODO.
  init();
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

  // Only l2 needs to be recalculated. TODO.
  init();
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
  // Only l2 needs to be recalculated. TODO.
  init();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotatePathComponents(
    const std::vector<size_t> &pathComponentIndexes,
    const Eigen::Vector3d &axis, const double &theta,
    const Eigen::Vector3d &center) {

  m_pathComponentInfo->rotatePathComponents(pathComponentIndexes, axis, theta,
                                            center);

  // Only l2 needs to be recalculated. TODO.
  init();
}

template <typename InstTree>
void DetectorInfo<InstTree>::movePathComponents(
    const std::vector<size_t> &pathComponentIndexes,
    const Eigen::Vector3d &offset) {

  m_pathComponentInfo->movePathComponents(pathComponentIndexes, offset);

  // Only l2 needs to be recalculated. TODO.
  init();
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
  return *m_pathComponentInfo;
}

#endif
