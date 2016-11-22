
#include "ComponentInfo.h"
#include "CompositeComponent.h"
#include "Detector.h"
#include "PathComponent.h"
#include <string>
#include <algorithm>
#include <iterator>

void ComponentInfo::registerDetector(Detector const *const comp) {

  const size_t newIndex = coreUpdate(comp);
  m_detectorComponentIndexes.push_back(newIndex);
}

void ComponentInfo::registerPathComponent(PathComponent const *const comp) {

  const size_t nextComponentIndex = coreUpdate(comp);
  const size_t nextPathIndex = m_pathComponentIndexes.size();
  m_entryPoints.push_back(comp->entryPoint());
  m_exitPoints.push_back(comp->exitPoint());
  m_pathComponentIndexes.push_back(nextComponentIndex);
  if (m_sampleIndex < 0 && comp->isSample()) {
    m_sampleIndex = nextPathIndex;
  } else if (m_sourceIndex < 0 && comp->isSource()) {
    m_sourceIndex = nextPathIndex;
  }
}

size_t ComponentInfo::registerComposite(CompositeComponent const *const comp) {
  return coreUpdate(comp);
}

void ComponentInfo::registerDetector(const Detector *const comp,
                                     size_t parentIndex) {
  const size_t newIndex = coreUpdate(comp, parentIndex);
  m_detectorComponentIndexes.push_back(newIndex);
  m_detectorIds.push_back(comp->detectorId());
}

void ComponentInfo::registerPathComponent(const PathComponent *const comp,
                                          size_t parentIndex) {
  const size_t nextComponentIndex = coreUpdate(comp, parentIndex);
  const size_t nextPathIndex = m_pathComponentIndexes.size();
  m_entryPoints.push_back(comp->entryPoint());
  m_exitPoints.push_back(comp->exitPoint());
  m_pathLengths.push_back(comp->length());
  m_pathComponentIndexes.push_back(nextComponentIndex);
  if (m_sampleIndex < 0 && comp->isSample()) {
    m_sampleIndex = nextPathIndex;
  } else if (m_sourceIndex < 0 && comp->isSource()) {
    m_sourceIndex = nextPathIndex;
  }
}

size_t ComponentInfo::registerComposite(const CompositeComponent *const comp,
                                        size_t parentIndex) {
  return coreUpdate(comp, parentIndex);
}

std::vector<ComponentProxy> ComponentInfo::proxies() { return m_proxies; }

const ComponentProxy &ComponentInfo::rootProxy() const { return m_proxies[0]; }

size_t ComponentInfo::componentSize() const { return m_proxies.size(); }

size_t ComponentInfo::detectorSize() const {
  return m_detectorComponentIndexes.size();
}

size_t ComponentInfo::pathSize() const { return m_pathComponentIndexes.size(); }

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

std::vector<double> ComponentInfo::pathLengths() const{
    return m_pathLengths;
}

size_t ComponentInfo::coreUpdate(Component const *const comp,
                                 size_t previousIndex) {
  size_t newIndex = m_proxies.size();
  m_componentIds.emplace_back(comp->componentId());
  m_proxies.emplace_back(previousIndex, comp);
  m_proxies[previousIndex].addChild(newIndex);
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

size_t ComponentInfo::coreUpdate(Component const *const comp) {
  const size_t newIndex = m_proxies.size();
  m_componentIds.emplace_back(comp->componentId());
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

std::vector<Eigen::Vector3d> ComponentInfo::startPositions() const {
  return m_positions;
}

std::vector<Eigen::Quaterniond> ComponentInfo::startRotations() const {
  return m_rotations;
}

std::vector<ComponentIdType> ComponentInfo::componentIds() const {
  return m_componentIds;
}

std::vector<DetectorIdType> ComponentInfo::detectorIds() const {
  return m_detectorIds;
}

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

const ComponentProxy &ComponentInfo::proxyAt(size_t index) const {
  return m_proxies[index];
}

void
ComponentInfo::fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const {

  const size_t nEntries = m_detectorIds.size();
  for (size_t i = 0; i < nEntries; ++i) {
    toFill.insert(std::make_pair(m_detectorIds[i], i));
  }
}

void ComponentInfo::fillComponentMap(
    std::map<ComponentIdType, size_t> &toFill) const {
  const size_t nEntries = m_componentIds.size();
  for (size_t i = 0; i < nEntries; ++i) {
    toFill.insert(std::make_pair(m_componentIds[i], i));
  }
}

int64_t ComponentInfo::sourcePathIndex() const { return m_sourceIndex; }

int64_t ComponentInfo::samplePathIndex() const { return m_sampleIndex; }
