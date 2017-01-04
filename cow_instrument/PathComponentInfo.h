#ifndef PATHCOMPONENTINFO_H
#define PATHCOMPONENTINFO_H

#include <vector>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "cow_ptr.h"

/**
 * PathComponentInfo type. Provides meta-data an behaviour for working with a
 * FlatTree at the PathComponent level
 * at the component level.
 */
template <typename InstTree> class PathComponentInfo {

public:

  explicit PathComponentInfo(std::shared_ptr<InstTree> &instrumentTree);
  explicit PathComponentInfo(std::shared_ptr<InstTree> &&instrumentTree);

  Eigen::Vector3d position(size_t pathComponentIndex) const;

  Eigen::Vector3d entryPoint(size_t pathComponentIndex) const;

  Eigen::Vector3d exitPoint(size_t pathComponentIndex) const;

  Eigen::Quaterniond rotation(size_t pathComponentIndex) const;

  const std::vector<Eigen::Vector3d> &const_entryPoints() const;

  const std::vector<Eigen::Vector3d> &const_exitPoints() const;

  const std::vector<double> &const_pathLengths() const;

  const InstTree &const_instrumentTree() const;

  void movePathComponent(size_t pathComponentIndex,
                         const Eigen::Vector3d &offset);

  void movePathComponents(const std::vector<size_t> &pathComponentIndexes,
                          const Eigen::Vector3d &offset);

  void rotatePathComponent(size_t pathComponentIndex,
                           const Eigen::Vector3d &axis, const double &theta,
                           const Eigen::Vector3d &center);

  void rotatePathComponents(const std::vector<size_t> &pathComponentIndexs,
                            const Eigen::Vector3d &axis, const double &theta,
                            const Eigen::Vector3d &center);

  bool operator==(const PathComponentInfo<InstTree> &other) const;

  bool operator!=(const PathComponentInfo<InstTree> &other) const;

private:
  /// initalization
  void init();
  const size_t m_nPathComponents;
  /// All path component entry points.
  CowPtr<std::vector<Eigen::Vector3d>> m_entryPoints;
  /// All path component exit points
  CowPtr<std::vector<Eigen::Vector3d>> m_exitPoints;
  /// All path component entry points.
  std::shared_ptr<const std::vector<double>> m_pathLengths;
  /// Locally (path component) indexed positions
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// Locally (path component) indexed rotations
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
  /// Path component indexes
  std::shared_ptr<const std::vector<size_t>> m_pathComponentIndexes;
  /// Shared instrument. This is the "owner"
  std::shared_ptr<InstTree> m_instrumentTree;
};

namespace {

template <typename U>
void pathComponentRangeCheck(size_t pathComponentIndex, const U &container) {
  if (pathComponentIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "PathComponent at index " << pathComponentIndex
           << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}
}

template <typename InstTree>
PathComponentInfo<InstTree>::PathComponentInfo(
    std::shared_ptr<InstTree> &instrumentTree)

    : m_nPathComponents(instrumentTree->nPathComponents()),
      m_entryPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree->startEntryPoints())),
      m_exitPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree->startExitPoints())),
      m_positions(
          std::make_shared<std::vector<Eigen::Vector3d>>(m_nPathComponents)),
      m_pathLengths(
          std::make_shared<std::vector<double>>(instrumentTree->pathLengths())),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nPathComponents)),
      m_pathComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->pathComponentIndexes())),
      m_instrumentTree(instrumentTree) {

  init();
}

template <typename InstTree>
PathComponentInfo<InstTree>::PathComponentInfo(
    std::shared_ptr<InstTree> &&instrumentTree)

    : m_nPathComponents(instrumentTree->nPathComponents()),
      m_entryPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree->startEntryPoints())),
      m_exitPoints(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree->startExitPoints())),
      m_positions(
          std::make_shared<std::vector<Eigen::Vector3d>>(m_nPathComponents)),
      m_pathLengths(
          std::make_shared<std::vector<double>>(instrumentTree->pathLengths())),
      m_rotations(
          std::make_shared<std::vector<Eigen::Quaterniond>>(m_nPathComponents)),
      m_pathComponentIndexes(std::make_shared<const std::vector<size_t>>(
          instrumentTree->pathComponentIndexes())),
      m_instrumentTree(std::move(instrumentTree)) {

  init();
}

template <typename InstTree> void PathComponentInfo<InstTree>::init() {
  // TODO. Do this without copying everything!
  std::vector<Eigen::Vector3d> allComponentPositions =
      m_instrumentTree->startPositions();
  std::vector<Eigen::Quaterniond> allComponentRotations =
      m_instrumentTree->startRotations();

  size_t i = 0;
  for (auto &compIndex : (*m_pathComponentIndexes)) {
    (*m_positions)[i] = allComponentPositions[compIndex];
    (*m_rotations)[i] = allComponentRotations[compIndex];
    ++i;
  }
}

template <typename InstTree>
Eigen::Vector3d
PathComponentInfo<InstTree>::position(size_t pathComponentIndex) const {
  pathComponentRangeCheck(pathComponentIndex, *m_positions);
  return (*m_positions)[pathComponentIndex];
}

template <typename InstTree>
Eigen::Vector3d
PathComponentInfo<InstTree>::entryPoint(size_t pathComponentIndex) const {
  pathComponentRangeCheck(pathComponentIndex, *m_entryPoints);
  return (*m_entryPoints)[pathComponentIndex];
}

template <typename InstTree>
Eigen::Vector3d
PathComponentInfo<InstTree>::exitPoint(size_t pathComponentIndex) const {
  pathComponentRangeCheck(pathComponentIndex, *m_exitPoints);
  return (*m_exitPoints)[pathComponentIndex];
}

template <typename InstTree>
Eigen::Quaterniond
PathComponentInfo<InstTree>::rotation(size_t pathComponentIndex) const {
  pathComponentRangeCheck(pathComponentIndex, *m_rotations);
  return (*m_rotations)[pathComponentIndex];
}

template <typename InstTree>
const std::vector<Eigen::Vector3d> &
PathComponentInfo<InstTree>::const_entryPoints() const {
  return (*m_entryPoints);
}

template <typename InstTree>
const std::vector<Eigen::Vector3d> &
PathComponentInfo<InstTree>::const_exitPoints() const {
  return (*m_exitPoints);
}

template <typename InstTree>
const std::vector<double> &
PathComponentInfo<InstTree>::const_pathLengths() const {
  return (*m_pathLengths);
}

template <typename InstTree>
const InstTree &PathComponentInfo<InstTree>::const_instrumentTree() const {
  return *m_instrumentTree;
}

template <typename InstTree>
void
PathComponentInfo<InstTree>::movePathComponent(size_t pathComponentIndex,
                                               const Eigen::Vector3d &offset) {

  (*m_positions)[pathComponentIndex] += offset;
  (*m_entryPoints)[pathComponentIndex] += offset;
  (*m_exitPoints)[pathComponentIndex] += offset;

}

template <typename InstTree>
void PathComponentInfo<InstTree>::movePathComponents(
    const std::vector<size_t> &pathComponentIndexes,
    const Eigen::Vector3d &offset) {

  for (auto &pathComponentIndex : pathComponentIndexes) {
    (*m_positions)[pathComponentIndex] += offset;
    (*m_entryPoints)[pathComponentIndex] += offset;
    (*m_exitPoints)[pathComponentIndex] += offset;
  }

}

template <typename InstTree>
void PathComponentInfo<InstTree>::rotatePathComponent(
    size_t pathComponentIndex, const Eigen::Vector3d &axis, const double &theta,
    const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();

  (*m_positions)[pathComponentIndex] =
      transform * (*m_positions)[pathComponentIndex];
  (*m_entryPoints)[pathComponentIndex] =
      transform * (*m_entryPoints)[pathComponentIndex];
  (*m_exitPoints)[pathComponentIndex] =
      transform * (*m_exitPoints)[pathComponentIndex];
  (*m_rotations)[pathComponentIndex] =
      rotation * (*m_rotations)[pathComponentIndex];

}

template <typename InstTree>
void PathComponentInfo<InstTree>::rotatePathComponents(
    const std::vector<size_t> &pathComponentIndexes,
    const Eigen::Vector3d &axis, const double &theta,
    const Eigen::Vector3d &center) {

  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();
  for (auto &pathComponentIndex : pathComponentIndexes) {

    (*m_positions)[pathComponentIndex] =
        transform * (*m_positions)[pathComponentIndex];
    (*m_entryPoints)[pathComponentIndex] =
        transform * (*m_entryPoints)[pathComponentIndex];
    (*m_exitPoints)[pathComponentIndex] =
        transform * (*m_exitPoints)[pathComponentIndex];
    (*m_rotations)[pathComponentIndex] =
        rotation * (*m_rotations)[pathComponentIndex];
  }

  // Would need to update detectorInfo?
}

template <typename InstTree>
bool PathComponentInfo<InstTree>::
operator==(const PathComponentInfo<InstTree> &other) const {

  // We only need to compare instruments
  return this->const_instrumentTree() == other.const_instrumentTree();
}

template <typename InstTree>
bool PathComponentInfo<InstTree>::
operator!=(const PathComponentInfo<InstTree> &other) const {

  return !this->operator==(other);
}

#endif
