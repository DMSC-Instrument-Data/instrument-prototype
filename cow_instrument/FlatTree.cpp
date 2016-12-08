#include "FlatTree.h"
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
void findKeyComponents(const Component &component, SOASource &info) {

  // Walk through and register all detectors on the store.
  component.registerContents(info);
}
}

void FlatTree::init() {

  findKeyComponents(*m_componentRoot, m_source);
  auto sourceIndex = m_source.sourcePathIndex();
  auto sampleIndex = m_source.samplePathIndex();
  if (sourceIndex < 0) {
    throw std::invalid_argument("Instrument has no marked source");
  }
  if (sampleIndex < 0) {
    throw std::invalid_argument("Instrument has no marked sample");
  }
  m_sourceIndex = sourceIndex;
  m_sampleIndex = sampleIndex;
}

FlatTree::FlatTree(std::shared_ptr<Component> componentRoot)
    : m_componentRoot(componentRoot) {
  init();
}

const ComponentProxy &FlatTree::rootProxy() const {
  return m_source.rootProxy();
}

void FlatTree::fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const {
  m_source.fillDetectorMap(toFill);
}

void
FlatTree::fillComponentMap(std::map<ComponentIdType, size_t> &toFill) const {
  m_source.fillComponentMap(toFill);
}

size_t FlatTree::sampleComponentIndex() const {
  return pathIndexToCompIndex(m_sampleIndex);
}

size_t FlatTree::sourceComponentIndex() const {
  return pathIndexToCompIndex(m_sourceIndex);
}

size_t FlatTree::samplePathIndex() const { return m_sampleIndex; }

size_t FlatTree::sourcePathIndex() const { return m_sourceIndex; }

size_t FlatTree::componentSize() const {
  return m_source.componentSize();
}

std::vector<size_t> FlatTree::subTreeIndexes(size_t proxyIndex) const {
  return m_source.subTreeIndexes(proxyIndex);
}

std::vector<Eigen::Vector3d> FlatTree::startPositions() const {
  // forwarding
  return m_source.startPositions();
}

std::vector<Eigen::Quaterniond> FlatTree::startRotations() const {
  // forwarding
  return m_source.startRotations();
}

std::vector<Eigen::Vector3d> FlatTree::startExitPoints() const {
  return m_source.startExitPoints();
}

std::vector<Eigen::Vector3d> FlatTree::startEntryPoints() const {
  return m_source.startEntryPoints();
}

std::vector<double> FlatTree::pathLengths() const {
  return m_source.pathLengths();
}

std::vector<size_t> FlatTree::detectorComponentIndexes() const {
  return m_source.detectorComponentIndexes();
}

size_t FlatTree::detIndexToCompIndex(size_t detectorIndex) const {
  return m_source.detIndexToCompIndex(detectorIndex);
}

size_t FlatTree::pathIndexToCompIndex(size_t pathIndex) const {
  return m_source.pathIndexToCompIndex(pathIndex);
}

std::shared_ptr<Component> FlatTree::rootComponent() const {
  return m_componentRoot;
}

size_t FlatTree::nDetectors() const { return m_source.detectorSize(); }

size_t FlatTree::nPathComponents() const { return m_source.pathSize(); }

const ComponentProxy &FlatTree::proxyAt(size_t index) const {
  return m_source.proxyAt(index);
}

std::vector<ComponentProxy>::const_iterator FlatTree::begin() const {
  return m_source.begin();
}
std::vector<ComponentProxy>::const_iterator FlatTree::end() const {
  return m_source.end();
}
std::vector<ComponentProxy>::const_iterator FlatTree::cbegin() const {
  return m_source.cbegin();
}
std::vector<ComponentProxy>::const_iterator FlatTree::cend() const {
  return m_source.cend();
}
