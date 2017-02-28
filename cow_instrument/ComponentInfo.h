#ifndef COMPONENT_INFO_H
#define COMPONENT_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "ComponentProxy.h"
#include "cow_ptr.h"
#include "ScanTime.h"

/**
 * ComponentInfo type. Provides meta-data an behaviour for working with a
 * FlatTree at the component level. Wraps DetectorInfo and PatchComponentInfo
 * and therefore
 * gives a Component only view of the data and associated operations.
 */
template <typename InstTree> class ComponentInfo {
public:
  explicit ComponentInfo(const std::shared_ptr<const InstTree> &instTree,
                         ScanTime scanTime = ScanTime{});
  explicit ComponentInfo(const std::shared_ptr<const InstTree> &&instTree,
                         ScanTime scanTime = ScanTime{});

  template <typename InstSptrType, typename TimeIndexesType,
            typename ScanTimesType, typename PositionsType,
            typename RotationsType>
  explicit ComponentInfo(InstSptrType &&instrumentTree,
                         TimeIndexesType &&timeIndexes,
                         ScanTimesType &&scanTimes, PositionsType &&positions,
                         RotationsType &&rotations);

  Eigen::Vector3d position(size_t componentIndex) const;

  Eigen::Quaterniond rotation(size_t componentIndex) const;

  size_t size() const;

  size_t componentSize() const;

  const InstTree &const_instrumentTree() const;

  void move(size_t componentIndex, const Eigen::Vector3d &offset);

  void rotate(size_t componentIndex, const Eigen::Vector3d &axis,
              const double &theta, const Eigen::Vector3d &center);

private:
  std::shared_ptr<const InstTree> m_instrumentTree;

  /// Locally (component) indexed positions
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// Locally (component) indexed rotations
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
  /// Linear index map (component indexed)
  std::shared_ptr<const std::vector<std::vector<size_t>>> m_linearIndexMap;
  /// Scan durations
  std::shared_ptr<const ScanTimes> m_durations;
  /// Path component information
  bool m_isScanning = false;
};

namespace {
template <typename InstSptrType>
std::shared_ptr<std::vector<std::vector<size_t>>>
makeDefaultComponentIndexes(InstSptrType &instrumentTree) {
  size_t size = instrumentTree->componentSize();
  auto indexes = std::make_shared<std::vector<std::vector<size_t>>>(size);
  for (size_t i = 0; i < size; ++i) {
    // Always one time index per component. Just need the component index.
    (*indexes)[i] = std::vector<size_t>(1, i);
  }
  return indexes;
}
}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    const std::shared_ptr<const InstTree> &&instrumentTree, ScanTime scanTime)
    : m_instrumentTree(std::move(instrumentTree)),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree->startRotations())),
      m_linearIndexMap(makeDefaultComponentIndexes(instrumentTree)),
      m_durations(std::make_shared<const ScanTimes>(1, scanTime)) {}

template <typename InstTree>
ComponentInfo<InstTree>::ComponentInfo(
    const std::shared_ptr<const InstTree> &instrumentTree, ScanTime scanTime)
    : m_instrumentTree(instrumentTree),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          m_instrumentTree->startPositions())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          m_instrumentTree->startRotations())),
      m_linearIndexMap(makeDefaultComponentIndexes(instrumentTree)),
      m_durations(std::make_shared<const ScanTimes>(1, scanTime)) {}

template <typename InstTree>
template <typename InstSptrType, typename TimeIndexesType,
          typename ScanTimesType, typename PositionsType,
          typename RotationsType>
ComponentInfo<InstTree>::ComponentInfo(InstSptrType &&instrumentTree,
                                       TimeIndexesType &&timeIndexes,
                                       ScanTimesType &&scanTimes,
                                       PositionsType &&positions,
                                       RotationsType &&rotations)
    : m_instrumentTree(std::forward<InstSptrType>(instrumentTree)),
      m_linearIndexMap(std::make_shared<std::vector<std::vector<size_t>>>(
          std::forward<TimeIndexesType>(timeIndexes))),
      m_durations(
          std::make_shared<ScanTimes>(std::forward<ScanTimesType>(scanTimes))),
      m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          std::forward<PositionsType>(positions))),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          std::forward<RotationsType>(rotations))) {

  if (m_positions->size() != m_rotations->size()) {
    throw std::invalid_argument(
        "The numbers of rotations and positions should match");
  }
}

template <typename InstTree>
Eigen::Vector3d ComponentInfo<InstTree>::position(size_t componentIndex) const {
  return (*m_positions)[componentIndex];
}

template <typename InstTree>
Eigen::Quaterniond
ComponentInfo<InstTree>::rotation(size_t componentIndex) const {
  return (*m_rotations)[componentIndex];
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
void ComponentInfo<InstTree>::move(size_t componentIndex,
                                   const Eigen::Vector3d &offset) {
  (*m_positions)[componentIndex] += offset;
}
template <typename InstTree>
void ComponentInfo<InstTree>::rotate(size_t componentIndex,
                                     const Eigen::Vector3d &axis,
                                     const double &theta,
                                     const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();

  (*m_positions)[componentIndex] = transform * (*m_positions)[componentIndex];
  (*m_rotations)[componentIndex] = rotation * (*m_rotations)[componentIndex];
}
#endif
