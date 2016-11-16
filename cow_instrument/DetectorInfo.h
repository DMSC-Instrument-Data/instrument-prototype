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
#include "PathFactory.h"
#include "Spectrum.h"

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

  void setMasked(size_t detectorIndex);

  bool isMasked(size_t detectorIndex) const;

  void setMonitor(size_t detectorIndex);

  bool isMonitor(size_t detectorIndex) const;

  double l2(size_t detectorIndex) const;

  Eigen::Vector3d position(size_t componentIndex) const;

  Eigen::Vector3d positionDetector(size_t detectorIndex) const;

  Eigen::Quaterniond rotation(size_t componentIndex) const;

  Eigen::Quaterniond rotationDetector(size_t detectorIndex) const;

  double l1(size_t detectorIndex) const;

  size_t size() const;

  size_t componentSize() const;

  const InstTree &const_instrumentTree() const;

  void move(size_t componentIndex, const Eigen::Vector3d &offset);

  void rotate(size_t componentIndex, const Eigen::Vector3d &axis,
              const double &theta, const Eigen::Vector3d &center);

  void moveDetector(size_t detectorIndex, const Eigen::Vector3d &offset);

  void rotateDetector(size_t detectorIndex, const Eigen::Vector3d &axis,
                      const double &theta, const Eigen::Vector3d &center);

  std::vector<Spectrum> makeSpectra() const;

  CowPtr<L2s> l2s() const;

private:
  void init();
  void initL2();
  void initL1();
  void moveComponent(size_t componentIndex, const Eigen::Vector3d &offset);
  void rotateComponent(size_t componentIndex, const Eigen::Vector3d &axis,
                       const double &theta, const Eigen::Vector3d &center);

  //------------------- MetaData -------------
  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked; // This could be copied upon instrument change
  CowPtr<MonitorFlags>
      m_isMonitor; // This could be copied upon instrument change

  //------------------- DerivedInfo
  CowPtr<L1s> m_l1;
  CowPtr<L2s> m_l2;

  CowPtr<const Paths> m_l2Paths;
  CowPtr<const Paths> m_l1Paths;

  // Instrument
  std::shared_ptr<const InstTree> m_instrumentTree;

  // All positions indexed by component index
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  // All rotations indexed by component index
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;

  CowPtr<std::vector<Eigen::Vector3d>> m_startEntryPoints;
  CowPtr<std::vector<Eigen::Vector3d>> m_startExitPoints;
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
  return std::sqrt(((a[0] - b[0]) * (a[0] - b[0])) +
                   ((a[1] - b[1]) * (a[1] - b[1])) +
                   ((a[2] - b[2]) * (a[2] - b[2])));
}
}

template <typename InstTree>
template <typename InstSptrType, typename PathFactoryType>
DetectorInfo<InstTree>::DetectorInfo(InstSptrType &&instrumentTree,
                                     PathFactoryType &&pathFactory)
    : m_l2Paths(pathFactory.createL2(*instrumentTree)),
      m_l1Paths(pathFactory.createL1(*instrumentTree)),
      m_nDetectors(instrumentTree->nDetectors()),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors)),
      m_instrumentTree(std::forward<InstSptrType>(instrumentTree)),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree->startRotations())),
      m_startEntryPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startEntryPoints())),
      m_startExitPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startExitPoints()))

{

  init();
}

template <typename InstTree>
void DetectorInfo<InstTree>::setMasked(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  (*m_isMasked)[detectorIndex] = true;
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

template <typename InstTree> void DetectorInfo<InstTree>::init() {

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

  // Loop over all detector indexes. We will have a path for each.
  for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
       ++detectorIndex) {

    size_t i = 0;
    const Path &path = (*m_l1Paths)[detectorIndex];
    if (path.size() < 2) {
      throw std::logic_error("Cannot have a L1 specified with less than 2 path "
                             "components (sample + source).");
    }

    double l1 = 0;
    // For each detector-l1-path calculate the total neutronic length

    l1 += m_instrumentTree->getPathComponent(path[i]).length(); // TODO
    for (i = 1; i < path.size(); ++i) {
      const PathComponent &current =
          m_instrumentTree->getPathComponent(path[i]);
      const PathComponent &previous =
          m_instrumentTree->getPathComponent(path[i - 1]);

      l1 += distance(current.entryPoint(), previous.exitPoint());
      l1 += current.length();
    }

    (*m_l1)[detectorIndex] = l1;
  }
}

template <typename InstTree> void DetectorInfo<InstTree>::initL2() {

  // Loop over all detector indexes. We will have a path for each.
  for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
       ++detectorIndex) {

    auto detectorPos = positionDetector(detectorIndex);
    size_t i = 0;
    const Path &path = (*m_l2Paths)[detectorIndex];
    if (path.size() < 1) {
      throw std::logic_error("Cannot have a L2 specified with less than 1 path "
                             "components (sample).");
    }
    double l2 = 0;

    l2 += m_instrumentTree->getPathComponent(path[i]).length();

    // For each detector-l2-path calculate the total neutronic length
    for (i = 1; i < path.size(); ++i) {
      const PathComponent &current =
          m_instrumentTree->getPathComponent(path[i]);
      const PathComponent &previous =
          m_instrumentTree->getPathComponent(path[i - 1]);
      l2 += distance(current.entryPoint(), previous.exitPoint());
      l2 += current.length();
    }
    l2 += distance(m_instrumentTree->getPathComponent(path[i - 1]).exitPoint(),
                   detectorPos);

    (*m_l2)[detectorIndex] = l2;
  }
}

template <typename InstTree>
double DetectorInfo<InstTree>::l2(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_l2.const_ref());
  return m_l2.const_ref()[detectorIndex];
}

template <typename InstTree>
Eigen::Vector3d DetectorInfo<InstTree>::position(size_t componentIndex) const {
  return (*m_positions)[componentIndex];
}

template <typename InstTree>
Eigen::Vector3d
DetectorInfo<InstTree>::positionDetector(size_t detectorIndex) const {
  return (*m_positions)[m_instrumentTree->detIndexToCompIndex(detectorIndex)];
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotation(size_t componentIndex) const {
  return (*m_rotations)[componentIndex];
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotationDetector(size_t detectorIndex) const {
  return (*m_rotations)[m_instrumentTree->detIndexToCompIndex(detectorIndex)];
}

template <typename InstTree>
double DetectorInfo<InstTree>::l1(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_l1.const_ref());
  return m_l1.const_ref()[detectorIndex];
}

template <typename InstTree> size_t DetectorInfo<InstTree>::size() const {
  return m_nDetectors;
}

template <typename InstTree>
size_t DetectorInfo<InstTree>::componentSize() const {
  return m_instrumentTree->componentSize();
}

template <typename InstTree>
const InstTree &DetectorInfo<InstTree>::const_instrumentTree() const {
  return *m_instrumentTree;
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveComponent(size_t componentIndex,
                                           const Eigen::Vector3d &offset) {

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);
  for (auto &index : indexes) {
    (*m_positions)[index] += offset;
  }
}

template <typename InstTree>
void DetectorInfo<InstTree>::move(size_t componentIndex,
                                  const Eigen::Vector3d &offset) {

  moveComponent(componentIndex, offset);

  // All other geometry-derived information is now also invalid. Very
  // important!
  init();

  // Meta-data should all still be valid.
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotate(size_t componentIndex,
                                    const Eigen::Vector3d &axis,
                                    const double &theta,
                                    const Eigen::Vector3d &center) {

  rotateComponent(componentIndex, axis, theta, center);
  // All other geometry-derived information is now also invalid. Very
  // important!
  init();

  // Meta-data should all still be valid.
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotateComponent(size_t componentIndex,
                                             const Eigen::Vector3d &axis,
                                             const double &theta,
                                             const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);
  for (auto &index : indexes) {
    (*m_positions)[index] = transform * (*m_positions)[index];
    (*m_rotations)[index] = rotation * (*m_rotations)[index];
  }
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetector(size_t detectorIndex,
                                          const Eigen::Vector3d &offset) {

  moveComponent(m_instrumentTree->detIndexToCompIndex(detectorIndex), offset);

  // Only l2 needs to be recalculated.
  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotateDetector(size_t detectorIndex,
                                            const Eigen::Vector3d &axis,
                                            const double &theta,
                                            const Eigen::Vector3d &center) {

  rotateComponent(m_instrumentTree->detIndexToCompIndex(detectorIndex), axis,
                  theta, center);

  // Only l2 needs to be recalculated.
  initL2();
}

template <typename InstTree>
std::vector<Spectrum> DetectorInfo<InstTree>::makeSpectra() const {
  std::vector<Spectrum> spectra;
  spectra.reserve(this->size());
  for (size_t i = 0; i < this->size(); ++i) {
    spectra.push_back(i);
  }
  return spectra;
}

template <typename InstTree> CowPtr<L2s> DetectorInfo<InstTree>::l2s() const {
  return m_l2;
}

#endif
