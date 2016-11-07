
#include "ComponentInfo.h"
#include "CompositeComponent.h"
#include "Detector.h"
#include "PathComponent.h"
#include <string>

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
  m_proxies.clear();
  m_detectorComponents.clear();
  m_pathComponents.clear();
  m_detectorComponentIndexes.clear();
  m_pathComponentIndexes.clear();
}

void ComponentInfo::registerDetector(Detector const *const comp) {

  const size_t newIndex = updateProxies(comp);
  m_detectorComponents.push_back(comp);
  m_detectorComponentIndexes.push_back(newIndex);
}

void ComponentInfo::registerPathComponent(PathComponent const *const comp) {

  const size_t newIndex = updateProxies(comp);
  m_pathComponents.push_back(comp);
  m_pathComponentIndexes.push_back(newIndex);
}

size_t ComponentInfo::registerComposite(CompositeComponent const *const comp) {
  return updateProxies(comp);
}

void ComponentInfo::registerDetector(const Detector *const comp,
                                     size_t parentIndex) {
  updateProxies(comp, parentIndex);
  m_detectorComponents.push_back(comp);
  m_detectorComponentIndexes.push_back(parentIndex);
}

void ComponentInfo::registerPathComponent(const PathComponent *const comp,
                                          size_t parentIndex) {
  updateProxies(comp, parentIndex);
  m_pathComponents.push_back(comp);
  m_pathComponentIndexes.push_back(parentIndex);
}

size_t ComponentInfo::registerComposite(const CompositeComponent *const comp,
                                        size_t parentIndex) {
  return updateProxies(comp, parentIndex);
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
    subtree.insert(subtree.end(), currentChildren.begin(),
                   currentChildren.end());
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

size_t ComponentInfo::updateProxies(Component const *const comp,
                                    size_t previousIndex) {
  size_t newIndex = m_proxies.size();
  m_proxies.emplace_back(previousIndex, comp);
  m_proxies[previousIndex].addChild(newIndex);
  return newIndex;
}

size_t ComponentInfo::updateProxies(Component const *const comp) {
  const size_t newIndex = m_proxies.size();
  m_proxies.emplace_back(comp);
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
  checkPathRange(pathComponentIndex, this->pathSize());
  return *m_pathComponents[pathComponentIndex];
}
