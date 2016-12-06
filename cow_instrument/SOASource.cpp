
#include "CompositeComponent.h"
#include "Detector.h"
#include "PathComponent.h"
#include "SOASource.h"
#include <string>
#include <algorithm>
#include <iterator>

void SOASource::registerDetector(Detector const *const comp) {

  const size_t newIndex = coreUpdate(comp);
  m_detectorComponentIndexes.push_back(newIndex);
}

void SOASource::registerPathComponent(PathComponent const *const comp) {

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

size_t SOASource::registerComposite(CompositeComponent const *const comp) {
  return coreUpdate(comp);
}

void SOASource::registerDetector(const Detector *const comp,
                                 size_t parentIndex) {
  const size_t newIndex = coreUpdate(comp, parentIndex);
  m_detectorComponentIndexes.push_back(newIndex);
  m_detectorIds.push_back(comp->detectorId());
}

void SOASource::registerPathComponent(const PathComponent *const comp,
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

size_t SOASource::registerComposite(const CompositeComponent *const comp,
                                    size_t parentIndex) {
  return coreUpdate(comp, parentIndex);
}

std::vector<ComponentProxy> SOASource::proxies() { return m_proxies; }

const ComponentProxy &SOASource::rootProxy() const { return m_proxies[0]; }

size_t SOASource::componentSize() const { return m_proxies.size(); }

size_t SOASource::detectorSize() const {
  return m_detectorComponentIndexes.size();
}

size_t SOASource::pathSize() const { return m_pathComponentIndexes.size(); }

std::vector<size_t> SOASource::pathComponentIndexes() const {
  return m_pathComponentIndexes;
}

std::vector<size_t> SOASource::detectorComponentIndexes() const {
  return m_detectorComponentIndexes;
}

size_t SOASource::detIndexToCompIndex(size_t detectorIndex) const {
  return m_detectorComponentIndexes[detectorIndex];
}

size_t SOASource::pathIndexToCompIndex(size_t pathIndex) const {
  return m_pathComponentIndexes[pathIndex];
}

std::vector<Eigen::Vector3d> SOASource::startEntryPoints() const {
  return m_entryPoints;
}

std::vector<Eigen::Vector3d> SOASource::startExitPoints() const {
  return m_exitPoints;
}

std::vector<double> SOASource::pathLengths() const { return m_pathLengths; }

size_t SOASource::coreUpdate(Component const *const comp,
                             size_t previousIndex) {
  size_t newIndex = m_proxies.size();
  m_componentIds.emplace_back(comp->componentId());
  m_proxies.emplace_back(previousIndex, comp->componentId());
  m_proxies[previousIndex].addChild(newIndex);
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

size_t SOASource::coreUpdate(Component const *const comp) {
  const size_t newIndex = m_proxies.size();
  m_componentIds.emplace_back(comp->componentId());
  m_proxies.emplace_back(comp->componentId());
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

std::vector<ComponentProxy>::const_iterator SOASource::begin() const {
  return m_proxies.begin();
}
std::vector<ComponentProxy>::const_iterator SOASource::end() const {
  return m_proxies.end();
}
std::vector<ComponentProxy>::const_iterator SOASource::cbegin() const {
  return m_proxies.cbegin();
}
std::vector<ComponentProxy>::const_iterator SOASource::cend() const {
  return m_proxies.cend();
}

bool SOASource::operator==(const SOASource &other) const {
  // We only need to compare the proxies.
  return m_proxies == other.m_proxies;
}

bool SOASource::operator!=(const SOASource &other) const {
  return !operator==(other);
}

std::vector<Eigen::Vector3d> SOASource::startPositions() const {
  return m_positions;
}

std::vector<Eigen::Quaterniond> SOASource::startRotations() const {
  return m_rotations;
}

std::vector<ComponentIdType> SOASource::componentIds() const {
  return m_componentIds;
}

std::vector<DetectorIdType> SOASource::detectorIds() const {
  return m_detectorIds;
}

std::vector<size_t> SOASource::subTreeIndexes(size_t proxyIndex) const {
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

const ComponentProxy &SOASource::proxyAt(size_t index) const {
  return m_proxies[index];
}

void
SOASource::fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const {

  const size_t nEntries = m_detectorIds.size();
  for (size_t i = 0; i < nEntries; ++i) {
    toFill.insert(std::make_pair(m_detectorIds[i], i));
  }
}

void
SOASource::fillComponentMap(std::map<ComponentIdType, size_t> &toFill) const {
  const size_t nEntries = m_componentIds.size();
  for (size_t i = 0; i < nEntries; ++i) {
    toFill.insert(std::make_pair(m_componentIds[i], i));
  }
}

int64_t SOASource::sourcePathIndex() const { return m_sourceIndex; }

int64_t SOASource::samplePathIndex() const { return m_sampleIndex; }

