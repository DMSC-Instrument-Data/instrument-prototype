
#include "ComponentInfo.h"
#include "CompositeComponent.h"
#include "Detector.h"
#include "PathComponent.h"
#include <string>
#include <algorithm>
#include <iterator>

namespace {

void checkDetectorRange(size_t detectorIndex, size_t limit) {
  if (detectorIndex >= limit) {
    throw std::invalid_argument(
        "Detector Index is outside range of indexes. Index is: " +
        std::to_string(detectorIndex));
  }
}

void checkPathRange(size_t pathIndex, size_t limit) {
  if (pathIndex >= limit) {
    throw std::invalid_argument(
        "PathComponent Index is outside range of indexes. Index is: " +
        std::to_string(pathIndex));
  }
}
}

void ComponentInfo::clear() {
  /* TODO. I think this operation is only in place to support the current
   mutable
   * use case. I think that the clear functionality should be removed.
   *

*/
  m_proxies.clear();
  m_detectorComponents.clear();
  m_pathComponents.clear();
  m_detectorComponentIndexes.clear();
  m_pathComponentIndexes.clear();
}

void ComponentInfo::registerDetector(Detector const *const comp) {

  const size_t newIndex = coreUpdate(comp);
  m_detectorComponents.push_back(comp);
  m_detectorComponentIndexes.push_back(newIndex);
}

void ComponentInfo::registerPathComponent(PathComponent const *const comp) {

  const size_t newIndex = coreUpdate(comp);
  m_pathComponents.push_back(comp);
  m_entryPoints.push_back(comp->entryPoint());
  m_exitPoints.push_back(comp->exitPoint());
  m_pathComponentIndexes.push_back(newIndex);
}

size_t ComponentInfo::registerComposite(CompositeComponent const *const comp) {
  return coreUpdate(comp);
}

void ComponentInfo::registerDetector(const Detector *const comp,
                                     size_t parentIndex) {
  coreUpdate(comp, parentIndex);
  m_detectorComponents.push_back(comp);
  m_detectorComponentIndexes.push_back(parentIndex);
}

void ComponentInfo::registerPathComponent(const PathComponent *const comp,
                                          size_t parentIndex) {
  size_t componentIndex = coreUpdate(comp, parentIndex);
  m_pathComponents.push_back(comp);
  m_entryPoints.push_back(comp->entryPoint());
  m_exitPoints.push_back(comp->exitPoint());
  m_pathComponentIndexes.push_back(componentIndex);
}

size_t ComponentInfo::registerComposite(const CompositeComponent *const comp,
                                        size_t parentIndex) {
  return coreUpdate(comp, parentIndex);
}

std::vector<ComponentProxy> ComponentInfo::proxies() { return m_proxies; }

const ComponentProxy &ComponentInfo::proxyAt(size_t index) const {
  return m_proxies[index];
}

const ComponentProxy &ComponentInfo::rootProxy() const { return m_proxies[0]; }

size_t ComponentInfo::componentSize() const { return m_proxies.size(); }

size_t ComponentInfo::detectorSize() const {
  return m_detectorComponents.size();
}

size_t ComponentInfo::pathSize() const { return m_pathComponents.size(); }

std::vector<size_t> ComponentInfo::subTreeIndexes(size_t proxyIndex) const {
  if (proxyIndex >= componentSize()) {
    throw std::invalid_argument("No subtree for proxy index: " +
                                std::to_string(proxyIndex));
  }

  std::vector<size_t> subtree = {proxyIndex};
  for (size_t index = 0; index < subtree.size(); ++index) {
    auto &currentProxy = m_proxies[subtree[index]];
    const auto &currentChildren = currentProxy.children();
    std::copy(currentChildren.begin(), currentChildren.end(),
              std::back_inserter(subtree));
  }
  return subtree;
}

std::vector<const Detector *> ComponentInfo::detectorComponents() const {
  return m_detectorComponents;
}

std::vector<const PathComponent *> ComponentInfo::pathComponents() const {
  return m_pathComponents;
}

std::vector<size_t> ComponentInfo::pathComponentIndexes() const {
  return m_pathComponentIndexes;
}

std::vector<size_t> ComponentInfo::detectorComponentIndexes() const {
  return m_detectorComponentIndexes;
}

size_t ComponentInfo::detIndexToCompIndex(size_t detectorIndex) const {
  return m_detectorComponentIndexes[detectorIndex];
}

size_t ComponentInfo::pathIndexToCompIndex(size_t pathIndex) const {
  return m_pathComponentIndexes[pathIndex];
}

std::vector<Eigen::Vector3d> ComponentInfo::startEntryPoints() const {
  return m_entryPoints;
}

std::vector<Eigen::Vector3d> ComponentInfo::startExitPoints() const {
  return m_exitPoints;
}

size_t ComponentInfo::coreUpdate(Component const *const comp,
                                 size_t previousIndex) {
  size_t newIndex = m_proxies.size();
  m_proxies.emplace_back(previousIndex, comp);
  m_proxies[previousIndex].addChild(newIndex);
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

size_t ComponentInfo::coreUpdate(Component const *const comp) {
  const size_t newIndex = m_proxies.size();
  m_proxies.emplace_back(comp);
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

std::vector<ComponentProxy>::const_iterator ComponentInfo::begin() const {
  return m_proxies.begin();
}
std::vector<ComponentProxy>::const_iterator ComponentInfo::end() const {
  return m_proxies.end();
}
std::vector<ComponentProxy>::const_iterator ComponentInfo::cbegin() const {
  return m_proxies.cbegin();
}
std::vector<ComponentProxy>::const_iterator ComponentInfo::cend() const {
  return m_proxies.cend();
}

bool ComponentInfo::operator==(const ComponentInfo &other) const {
  // We only need to compare the proxies.
  return m_proxies == other.m_proxies;
}

bool ComponentInfo::operator!=(const ComponentInfo &other) const {
  return !operator==(other);
}

const Detector &ComponentInfo::detectorComponentAt(size_t detectorIndex) const {
  checkDetectorRange(detectorIndex, this->detectorSize());
  return *m_detectorComponents[detectorIndex];
}

const PathComponent &
ComponentInfo::pathComponentAt(size_t pathComponentIndex) const {
  // checkPathRange(pathComponentIndex, this->pathSize());
  return *m_pathComponents[pathComponentIndex];
}

std::vector<Eigen::Vector3d> ComponentInfo::startPositions() const {
  return m_positions;
}

std::vector<Eigen::Quaterniond> ComponentInfo::startRotations() const {
  return m_rotations;
}
