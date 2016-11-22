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

}

void InstrumentTree::init() {

  findKeyComponents(*m_componentRoot, m_componentInfo);
  auto sourceIndex = m_componentInfo.sourcePathIndex();
  auto sampleIndex = m_componentInfo.samplePathIndex();
  if (sourceIndex < 0) {
    throw std::invalid_argument("Instrument has no marked source");
  }
  if (sampleIndex < 0) {
    throw std::invalid_argument("Instrument has no marked sample");
  }
  m_sourceIndex = sourceIndex;
  m_sampleIndex = sampleIndex;
}

InstrumentTree::InstrumentTree(std::shared_ptr<Component> componentRoot)
    : m_componentRoot(componentRoot) {
  init();
}

const ComponentProxy &InstrumentTree::rootProxy() const {
  return m_componentInfo.rootProxy();
}

void InstrumentTree::fillDetectorMap(
    std::map<DetectorIdType, size_t> &toFill) const {
  m_componentInfo.fillDetectorMap(toFill);
}

void InstrumentTree::fillComponentMap(
    std::map<ComponentIdType, size_t> &toFill) const {
  m_componentInfo.fillComponentMap(toFill);
}

size_t InstrumentTree::sampleComponentIndex() const {
  return pathIndexToCompIndex(m_sampleIndex);
}

size_t InstrumentTree::sourceComponentIndex() const {
  return pathIndexToCompIndex(m_sourceIndex);
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

std::vector<Eigen::Vector3d> InstrumentTree::startExitPoints() const {
  return m_componentInfo.startExitPoints();
}

std::vector<Eigen::Vector3d> InstrumentTree::startEntryPoints() const {
  return m_componentInfo.startEntryPoints();
}

std::vector<double> InstrumentTree::pathLengths() const{
    return m_componentInfo.pathLengths();
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
