#ifndef COMPONENT_INFO_H
#define COMPONENT_INFO_H

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
 * ComponentInfo type. Provides meta-data an behaviour for working with a
 * FlatTree
 * at the component level. Meta-data is provided per component. Add detectors
 * are components.
 */
template <typename InstTree> class ComponentInfo {
public:
  template <typename InstSptrType>
  explicit ComponentInfo(InstSptrType &&instrumentTree);

  Eigen::Vector3d position(size_t componentIndex) const;

  Eigen::Vector3d positionDetector(size_t detectorIndex) const;

  Eigen::Quaterniond rotation(size_t componentIndex) const;

  Eigen::Quaterniond rotationDetector(size_t detectorIndex) const;

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


private:
  void moveComponent(size_t componentIndex, const Eigen::Vector3d &offset);
  void rotateComponent(size_t componentIndex, const Eigen::Vector3d &axis,
                       const double &theta, const Eigen::Vector3d &center);

  //------------------- MetaData -------------
  const size_t m_nDetectors;

  //------------------- DerivedInfo

  /// Instrument tree.
  std::shared_ptr<const InstTree> m_instrumentTree;
  /// All positions indexed by component index. Owned by ComponentInfo.
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// All rotations indexed by component index. Owned by ComponentInfo.
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
};

namespace {

template <typename U>
void detectorRangeCheck2(size_t detectorIndex, const U &container) {
  if (detectorIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "Detector at index " << detectorIndex << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}

double distance2(const Eigen::Vector3d &a, const Eigen::Vector3d &b) {
  return std::sqrt(((a[0] - b[0]) * (a[0] - b[0])) +
                   ((a[1] - b[1]) * (a[1] - b[1])) +
                   ((a[2] - b[2]) * (a[2] - b[2])));
}
}

template <typename InstTree>
template <typename InstSptrType>
ComponentInfo<InstTree>::ComponentInfo(InstSptrType &&instrumentTree)
    : m_nDetectors(instrumentTree->nDetectors()),
      m_instrumentTree(std::forward<InstSptrType>(instrumentTree)),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree->startRotations()))

{}

template <typename InstTree>
Eigen::Vector3d ComponentInfo<InstTree>::position(size_t componentIndex) const {
  return (*m_positions)[componentIndex];
}

template <typename InstTree>
Eigen::Vector3d
ComponentInfo<InstTree>::positionDetector(size_t detectorIndex) const {
  return (*m_positions)[m_instrumentTree->detIndexToCompIndex(detectorIndex)];
}

template <typename InstTree>
Eigen::Quaterniond
ComponentInfo<InstTree>::rotation(size_t componentIndex) const {
  return (*m_rotations)[componentIndex];
}

template <typename InstTree>
Eigen::Quaterniond
ComponentInfo<InstTree>::rotationDetector(size_t detectorIndex) const {
  return (*m_rotations)[m_instrumentTree->detIndexToCompIndex(detectorIndex)];
}

template <typename InstTree> size_t ComponentInfo<InstTree>::size() const {
  return m_nDetectors;
}

template <typename InstTree>
size_t ComponentInfo<InstTree>::componentSize() const {
  return m_instrumentTree->componentSize();
}

template <typename InstTree>
const InstTree &ComponentInfo<InstTree>::const_instrumentTree() const {
  return *m_instrumentTree;
}

template <typename InstTree>
void ComponentInfo<InstTree>::moveComponent(size_t componentIndex,
                                            const Eigen::Vector3d &offset) {

  const std::vector<size_t> indexes =
      m_instrumentTree->subTreeIndexes(componentIndex);
  for (auto &index : indexes) {
    (*m_positions)[index] += offset;
    // TODO m_entry and m_exit points should also be translated here!
  }
}

template <typename InstTree>
void ComponentInfo<InstTree>::move(size_t componentIndex,
                                   const Eigen::Vector3d &offset) {

  moveComponent(componentIndex, offset);

}

template <typename InstTree>
void ComponentInfo<InstTree>::rotate(size_t componentIndex,
                                     const Eigen::Vector3d &axis,
                                     const double &theta,
                                     const Eigen::Vector3d &center) {

  rotateComponent(componentIndex, axis, theta, center);

}

template <typename InstTree>
void ComponentInfo<InstTree>::rotateComponent(size_t componentIndex,
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
    // TODO m_entry and m_exit points should also be translated here!
  }
}

template <typename InstTree>
void ComponentInfo<InstTree>::moveDetector(size_t detectorIndex,
                                           const Eigen::Vector3d &offset) {

  moveComponent(m_instrumentTree->detIndexToCompIndex(detectorIndex), offset);

}

template <typename InstTree>
void ComponentInfo<InstTree>::rotateDetector(size_t detectorIndex,
                                             const Eigen::Vector3d &axis,
                                             const double &theta,
                                             const Eigen::Vector3d &center) {

  rotateComponent(m_instrumentTree->detIndexToCompIndex(detectorIndex), axis,
                  theta, center);
}

template <typename InstTree>
std::vector<Spectrum> ComponentInfo<InstTree>::makeSpectra() const {
  std::vector<Spectrum> spectra;
  spectra.reserve(this->size());
  for (size_t i = 0; i < this->size(); ++i) {
    spectra.push_back(i);
  }
  return spectra;
}

#endif
