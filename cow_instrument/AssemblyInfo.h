#ifndef ASSEMBLYINFO_H
#define ASSEMBLYINFO_H

#include <vector>
#include <memory>
#include "cow_ptr.h"
#include "Eigen/Core"
/**
 * Provides a component assembly
 */
template <typename InstTree> class AssemblyInfo {
public:
  AssemblyInfo();
  explicit AssemblyInfo(const InstTree &instrumentTree);
  Eigen::Quaterniond rotation(size_t assemblyIndex) const;
  Eigen::Vector3d position(size_t assemblyIndex) const;
  void moveAssemblyComponents(const std::vector<size_t> &assemblyIndexes,
                              const Eigen::Vector3d &offset);
  void rotateAssemblyComponents(const std::vector<size_t> &assemblyIndexes,
                                const Eigen::Vector3d &axis,
                                const double &theta,
                                const Eigen::Vector3d &center);

private:
  /// Inverted map to get branch node indexes from component indexes
  std::vector<int64_t> m_componentToBranchNodeIndex;
  /// Locally (branch node) indexed positions
  CowPtr<std::vector<Eigen::Vector3d>> m_positions;
  /// Locally (branch node) indexed rotations
  CowPtr<std::vector<Eigen::Quaterniond>> m_rotations;
};

template <typename InstTree>
AssemblyInfo<InstTree>::AssemblyInfo(const InstTree &instrumentTree)
    : m_positions(std::make_shared<std::vector<Eigen::Vector3d>>(
          instrumentTree.nBranchNodeComponents())),
      m_rotations(std::make_shared<std::vector<Eigen::Quaterniond>>(
          instrumentTree.nBranchNodeComponents())) {
  /*
   * We should fix that so that temporary copies of the rotation and
   * position vectors are not required. i.e. make them return by const ref.
   * However I'm not doing that at the moment because the Mocking
   * layer has grown significantly and these changes will make it quite a lot
   * more complicated
   */
  std::vector<Eigen::Vector3d> allComponentPositions =
      instrumentTree.startPositions();
  std::vector<Eigen::Quaterniond> allComponentRotations =
      instrumentTree.startRotations();

  size_t i = 0;
  auto assemblyComponentIndexes = instrumentTree.branchNodeComponentIndexes();
  for (auto &compIndex : assemblyComponentIndexes) {
    (*m_positions)[i] = allComponentPositions[compIndex];
    (*m_rotations)[i] = allComponentRotations[compIndex];
    ++i;
  }
}

template <typename InstTree>
Eigen::Vector3d AssemblyInfo<InstTree>::position(size_t assemblyIndex) const {
  return (*m_positions)[assemblyIndex];
}

template <typename InstTree>
Eigen::Quaterniond
AssemblyInfo<InstTree>::rotation(size_t assemblyIndex) const {
  return (*m_rotations)[assemblyIndex];
}

template <typename InstTree>
void AssemblyInfo<InstTree>::moveAssemblyComponents(
    const std::vector<size_t> &assemblyIndexes, const Eigen::Vector3d &offset) {

  /*
   * Warning. This method does not translate changes down to sub-components of
   * the assembly. To do that use ComponentInfo.
   * AssemblyInfo should only be used internally by ComponentInfo to avoid
   * danger.
   */
  for (auto &assemblyIndex : assemblyIndexes) {
    (*m_positions)[assemblyIndex] += offset;
  }
}
template <typename InstTree>
void AssemblyInfo<InstTree>::rotateAssemblyComponents(
    const std::vector<size_t> &assemblyIndexes, const Eigen::Vector3d &axis,
    const double &theta, const Eigen::Vector3d &center) {

  /*
   * Warning. This method does not apply changes down to sub-components of the
   * assembly. To do that use ComponentInfo.
   * AssemblyInfo should only be used internally by ComponentInfo to avoid
   * danger.
   */
  using namespace Eigen;
  const auto transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  const auto rotation = transform.rotation();
  for (auto &assemblyIndex : assemblyIndexes) {

    (*m_positions)[assemblyIndex] = transform * (*m_positions)[assemblyIndex];
    (*m_rotations)[assemblyIndex] = rotation * (*m_rotations)[assemblyIndex];
  }
}
#
#endif
