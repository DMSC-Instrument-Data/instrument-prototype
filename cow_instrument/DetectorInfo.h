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
  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked;
  CowPtr<MonitorFlags> m_isMonitor;

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
    : m_nDetectors(instrumentTree->nDetectors()),
      m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
      m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
      m_componentInfo(std::forward<InstSptrType>(instrumentTree),
                      std::forward<PathFactoryType>(pathFactory)) {}

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

template <typename InstTree>
double DetectorInfo<InstTree>::l2(size_t detectorIndex) const {
  return m_componentInfo.l2(detectorIndex);
}

template <typename InstTree>
Eigen::Vector3d DetectorInfo<InstTree>::position(size_t componentIndex) const {
  return m_componentInfo.position(componentIndex);
}

template <typename InstTree>
Eigen::Vector3d
DetectorInfo<InstTree>::positionDetector(size_t detectorIndex) const {

  return m_componentInfo.positionDetector(detectorIndex);
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotation(size_t componentIndex) const {
  return m_componentInfo.rotation(componentIndex);
}

template <typename InstTree>
Eigen::Quaterniond
DetectorInfo<InstTree>::rotationDetector(size_t detectorIndex) const {
  return m_componentInfo.rotationDetector(detectorIndex);
}

template <typename InstTree>
double DetectorInfo<InstTree>::l1(size_t detectorIndex) const {
  return m_componentInfo.l1(detectorIndex);
}

template <typename InstTree> size_t DetectorInfo<InstTree>::size() const {
  return m_componentInfo.size();
}

template <typename InstTree>
size_t DetectorInfo<InstTree>::componentSize() const {
  return m_componentInfo.const_instrumentTree().componentSize();
}

template <typename InstTree>
const InstTree &DetectorInfo<InstTree>::const_instrumentTree() const {
  return m_componentInfo.const_instrumentTree();
}

template <typename InstTree>
void DetectorInfo<InstTree>::move(size_t componentIndex,
                                  const Eigen::Vector3d &offset) {

  m_componentInfo.move(componentIndex, offset);
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotate(size_t componentIndex,
                                    const Eigen::Vector3d &axis,
                                    const double &theta,
                                    const Eigen::Vector3d &center) {

  m_componentInfo.rotate(componentIndex, axis, theta, center);
}

template <typename InstTree>
void DetectorInfo<InstTree>::moveDetector(size_t detectorIndex,
                                          const Eigen::Vector3d &offset) {

  moveComponent(
      m_componentInfo.const_instrumentTree().detIndexToCompIndex(detectorIndex),
      offset);

  // Only l2 needs to be recalculated.
  m_componentInfo.initL2();
}

template <typename InstTree>
void DetectorInfo<InstTree>::rotateDetector(size_t detectorIndex,
                                            const Eigen::Vector3d &axis,
                                            const double &theta,
                                            const Eigen::Vector3d &center) {

  rotateComponent(
      m_componentInfo.const_instrumentTree().detIndexToCompIndex(detectorIndex),
      axis, theta, center);

  // Only l2 needs to be recalculated.
  m_componentInfo.initL2();
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
  return m_componentInfo.l2s();
}

#endif
