#include "InstrumentTree.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "PathComponent.h"
#include <utility>
#include <string>
#include <algorithm>
#include <iterator>

namespace {

/**
 * @brief findKeyComponents
 * @param component
 * @param detectorStore
 * @param pathStore
 */
void findKeyComponents(const Component &component, ComponentInfo &info) {

  // Walk through and register all detectors on the store.
  component.registerContents(info);
}

bool findSource(const PathComponent *item) { return item->isSource(); }

bool findSample(const PathComponent *item) { return item->isSample(); }
}

void InstrumentTree::init() {

  findKeyComponents(*m_componentRoot, m_componentInfo);

  auto pathComponents = m_componentInfo.pathComponents();
  const auto begin_pathVec = pathComponents.cbegin();
  const auto end_pathVec = pathComponents.cend();
  auto sourceIt = std::find_if(begin_pathVec, end_pathVec, findSource);
  auto sampleIt = std::find_if(begin_pathVec, end_pathVec, findSample);
  if (sourceIt == end_pathVec) {
    throw std::invalid_argument("Instrument has no marked source");
  }
  if (sampleIt == end_pathVec) {
    throw std::invalid_argument("Instrument has no marked sample");
  }
  m_sourceIndex = std::distance(begin_pathVec, sourceIt);
  m_sampleIndex = std::distance(begin_pathVec, sampleIt);

  // Should we shrink to fit to reduce excess capacity?
  // m_detectorVec.shrink_to_fit(); This could be costly
}

InstrumentTree::InstrumentTree(std::shared_ptr<Component> componentRoot)
    : m_componentRoot(componentRoot) {
  init();
}

const ComponentProxy &InstrumentTree::rootProxy() const {
  return m_componentInfo.rootProxy();
}

const Detector &InstrumentTree::getDetector(size_t detectorIndex) const {
  return m_componentInfo.detectorComponentAt(detectorIndex);
}

const PathComponent &
InstrumentTree::getPathComponent(size_t pathComponentIndex) const {
  return m_componentInfo.pathComponentAt(pathComponentIndex);
};

void InstrumentTree::fillDetectorMap(
    std::map<DetectorIdType, size_t> &toFill) const {
  for (size_t index = 0; index < m_componentInfo.detectorComponents().size();
       ++index) {
    toFill.insert(std::make_pair(
        m_componentInfo.detectorComponents()[index]->detectorId(), index));
  }
}

const PathComponent &InstrumentTree::source() const {
  return m_componentInfo.pathComponentAt(m_sourceIndex);
}

const PathComponent &InstrumentTree::sample() const {
  return m_componentInfo.pathComponentAt(m_sampleIndex);
}

size_t InstrumentTree::samplePathIndex() const { return m_sampleIndex; }

size_t InstrumentTree::sourcePathIndex() const { return m_sourceIndex; }

size_t InstrumentTree::componentSize() const {
  return m_componentInfo.componentSize();
}

std::vector<size_t> InstrumentTree::subTreeIndexes(size_t proxyIndex) const {
  return m_componentInfo.subTreeIndexes(proxyIndex);
}

std::vector<Eigen::Vector3d> InstrumentTree::startPositions() const {
  // forwarding
  return m_componentInfo.startPositions();
}

std::vector<Eigen::Quaterniond> InstrumentTree::startRotations() const {
  // forwarding
  return m_componentInfo.startRotations();
}

size_t InstrumentTree::detIndexToCompIndex(size_t detectorIndex) const {
  return m_componentInfo.detIndexToCompIndex(detectorIndex);
}

size_t InstrumentTree::pathIndexToCompIndex(size_t pathIndex) const {
  return m_componentInfo.pathIndexToCompIndex(pathIndex);
}

std::shared_ptr<Component> InstrumentTree::rootComponent() const {
  return m_componentRoot;
}

size_t InstrumentTree::nDetectors() const {
  return m_componentInfo.detectorSize();
}

size_t InstrumentTree::nPathComponents() const {
  return m_componentInfo.pathSize();
}

const ComponentProxy &InstrumentTree::proxyAt(size_t index) const {
  return m_componentInfo.proxyAt(index);
}

std::vector<ComponentProxy>::const_iterator InstrumentTree::begin() const {
  return m_componentInfo.begin();
}
std::vector<ComponentProxy>::const_iterator InstrumentTree::end() const {
  return m_componentInfo.end();
}
std::vector<ComponentProxy>::const_iterator InstrumentTree::cbegin() const {
  return m_componentInfo.cbegin();
}
std::vector<ComponentProxy>::const_iterator InstrumentTree::cend() const {
  return m_componentInfo.cend();
}
