#ifndef ASSEMBLYINFO_H
#define ASSEMBLYINFO_H
#include "ComponentInfo.h"
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
  explicit AssemblyInfo(
      std::shared_ptr<const ComponentInfo<InstTree>> componentInfo);
  Eigen::Quaterniond rotation(size_t assemblyIndex) const;
  Eigen::Vector3d position(size_t assemblyIndex) const;
  void moveAssemblyComponent(const size_t assemblyIndex,
                             const Eigen::Vector3d &offset);
  void rotateAssemblyComponent(const size_t assemblyIndex,
                               const Eigen::Vector3d &axis, const double &theta,
                               const Eigen::Vector3d &center);

private:
  std::shared_ptr<const ComponentInfo<InstTree>> m_componentInfo;
  /// Get the component indexes corresponding to branch nodes
  std::shared_ptr<const std::vector<size_t>> m_assemblyComponentIndexes;
};

template <typename InstTree>
AssemblyInfo<InstTree>::AssemblyInfo(
    std::shared_ptr<const ComponentInfo<InstTree>> componentInfo)
    : m_componentInfo(componentInfo),
      m_assemblyComponentIndexes(std::make_shared<const std::vector<size_t>>(
          componentInfo->const_instrumentTree().branchNodeComponentIndexes())) {
}

template <typename InstTree>
Eigen::Vector3d AssemblyInfo<InstTree>::position(size_t assemblyIndex) const {
  return m_componentInfo->position(m_assemblyComponentIndexes[assemblyIndex]);
}

template <typename InstTree>
Eigen::Quaterniond
AssemblyInfo<InstTree>::rotation(size_t assemblyIndex) const {
  return m_componentInfo->rotation(m_assemblyComponentIndexes[assemblyIndex]);
}

template <typename InstTree>
void
AssemblyInfo<InstTree>::moveAssemblyComponent(const size_t assemblyIndex,
                                              const Eigen::Vector3d &offset) {

  const auto topComponentIndex = m_assemblyComponentIndexes[assemblyIndex];
  const auto componentsToMove =
      m_componentInfo->const_instrumentTree().subTreeIndexes(topComponentIndex);
  for (auto &compIndex : componentsToMove) {
    // TODO. It's likely this would be better done as part of ComponentInfo
    // directly
    m_componentInfo->move(compIndex, offset);
  }
}
template <typename InstTree>
void AssemblyInfo<InstTree>::rotateAssemblyComponent(
    const size_t assemblyIndex, const Eigen::Vector3d &axis,
    const double &theta, const Eigen::Vector3d &center) {

  const auto topComponentIndex = m_assemblyComponentIndexes[assemblyIndex];
  const auto componentsToRotate =
      m_componentInfo->const_instrumentTree().subTreeIndexes(topComponentIndex);
  for (auto &compIndex : componentsToRotate) {
    // TODO. It's likely this would be better done as part of ComponentInfo
    // directly
    m_componentInfo->rotate(compIndex, axis, theta, center);
  }
}
#
#endif
