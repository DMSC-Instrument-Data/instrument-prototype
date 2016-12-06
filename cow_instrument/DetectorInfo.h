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
#include "ComponentInfo.h"
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

  Eigen::Vector3d positionDetector(size_t detectorIndex) const;

  Eigen::Quaterniond rotationDetector(size_t detectorIndex) const;

  double l1(size_t detectorIndex) const;

  size_t detectorSize() const;

  const InstTree &const_instrumentTree() const;


  void moveDetector(size_t detectorIndex, const Eigen::Vector3d &offset);

  void rotateDetector(size_t detectorIndex, const Eigen::Vector3d &axis,
                      const double &theta, const Eigen::Vector3d &center);

  std::vector<Spectrum> makeSpectra() const;

  CowPtr<L2s> l2s() const;

  const ComponentInfo<InstTree> &componentInfo() const;

private:
  void init();
  void initL2();
  void initL1();

  //------------------- Metadata -------------
  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked;
  CowPtr<MonitorFlags> m_isMonitor;

  //------------------- DerivedInfo -------------
  CowPtr<L1s> m_l1;
  CowPtr<L2s> m_l2;
  CowPtr<const Paths> m_l2Paths;
  CowPtr<const Paths> m_l1Paths;
  /// All path component entry points.
  CowPtr<std::vector<Eigen::Vector3d>> m_startEntryPoints;
  /// All path component exit points
  CowPtr<std::vector<Eigen::Vector3d>> m_startExitPoints;
  /// All path lengths
  std::shared_ptr<const std::vector<double>> m_pathLengths; // Shouldn't change
  /// Component indexes per detector index
  std::shared_ptr<const std::vector<size_t>> m_detectorComponentIndexes;
  /// Component info
  ComponentInfo<InstTree> m_componentInfo;
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
      m_l1(std::make_shared<L1s>(m_nDetectors)),
      m_l2(std::make_shared<L2s>(m_nDetectors)),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_startEntryPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree->startEntryPoints())),
      m_startExitPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree->startExitPoints())),
      m_pathLengths(std::make_shared<const std::vector<double>>(
          instrumentTree->pathLengths())),
      m_detectorComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->detectorComponentIndexes())),
      m_componentInfo(std::forward<InstSptrType>(instrumentTree)) {

  init();
}

template <typename InstTree>
void DetectorInfo<InstTree>::setMasked(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  (*m_isMasked)[detectorIndex] = true;
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

    double l1 = (*m_pathLengths)[path[i]];
    for (i = 1; i < path.size(); ++i) {

      l1 += distance((*m_startEntryPoints)[path[i]],
                     (*m_startExitPoints)[path[i - 1]]);
      l1 += (*m_pathLengths)[path[i]];
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
    double l2 = (*m_pathLengths)[path[i]];

    // For each detector-l2-path calculate the total neutronic length
    for (i = 1; i < path.size(); ++i) {
      l2 += distance((*m_startEntryPoints)[path[i]],
                     (*m_startExitPoints)[path[i - 1]]);
      l2 += (*m_pathLengths)[path[i]];
    }
    l2 += distance((*m_startExitPoints)[path[i - 1]], detectorPos);

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

  return m_componentInfo.position((*m_detectorComponentIndexes)[detectorIndex]);
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotationDetector(size_t detectorIndex) const {
  return m_componentInfo.rotation((*m_detectorComponentIndexes)[detectorIndex]);
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
  return m_componentInfo.const_instrumentTree();
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetector(size_t detectorIndex,
                                          const Eigen::Vector3d &offset) {

  moveComponent((*m_detectorComponentIndexes)[detectorIndex]);

  // Only l2 needs to be recalculated.
  initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotateDetector(size_t detectorIndex,
                                            const Eigen::Vector3d &axis,
                                            const double &theta,
                                            const Eigen::Vector3d &center) {

  rotateComponent((*m_detectorComponentIndexes)[detectorIndex]);

  // Only l2 needs to be recalculated.
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
const ComponentInfo<InstTree> &DetectorInfo<InstTree>::componentInfo() const {
  return m_componentInfo;
}

#endif
