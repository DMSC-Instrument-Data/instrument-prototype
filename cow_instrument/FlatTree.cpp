#include "CompositeComponent.h"
#include "Detector.h"
#include "FlatTree.h"
#include "LinkedTreeParser.h"
#include "PathComponent.h"
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <iterator>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace {

/**
 * @brief findKeyComponents
 * @param component
 * @param detectorStore
 * @param pathStore
 */
void findKeyComponents(const Component &component, LinkedTreeParser &info) {

  // Walk through and register all detectors on the store.
  component.registerContents(info);
}
}

FlatTree::FlatTree(std::shared_ptr<Component> componentRoot)
    : m_componentRoot(componentRoot) {

  LinkedTreeParser treeParser;
  findKeyComponents(*m_componentRoot, treeParser);
  auto sourceIndex = treeParser.sourcePathIndex();
  auto sampleIndex = treeParser.samplePathIndex();
  if (sourceIndex < 0) {
    throw std::invalid_argument("Instrument has no marked source");
  }
  if (sampleIndex < 0) {
    throw std::invalid_argument("Instrument has no marked sample");
  }
  m_sourceIndex = sourceIndex;
  m_sampleIndex = sampleIndex;
  m_proxies = treeParser.proxies();
  m_positions = treeParser.startPositions();
  m_rotations = treeParser.startRotations();
  m_componentIds = treeParser.componentIds();
  m_entryPoints = treeParser.startEntryPoints();
  m_exitPoints = treeParser.startExitPoints();
  m_pathLengths = treeParser.pathLengths();
  m_pathComponentIndexes = treeParser.pathComponentIndexes();
  m_detectorComponentIndexes = treeParser.detectorComponentIndexes();
  m_detectorIds = treeParser.detectorIds();
}

/**
 * @brief FlatTree::FlatTree
 *
 * Prototype new constructor that bypasses the need to do anything with the
 *Component virtual
 * hierachy at all.
 *
 * Complexity of this constructor suggests that a constructional helper is
 *missing.
 *
 * @param proxies
 * @param positions
 * @param rotations
 * @param componentIds
 * @param entryPoints
 * @param exitPoints
 * @param pathLengths
 * @param pathComponentIndexes
 * @param detectorComponentIndexes
 * @param detectorIds
 * @param sourceIndex
 * @param sampleIndex
 */
FlatTree::FlatTree(std::vector<ComponentProxy> &&proxies,
                   std::vector<Eigen::Vector3d> &&positions,
                   std::vector<Eigen::Quaterniond> &&rotations,
                   std::vector<ComponentIdType> &&componentIds,
                   std::vector<Eigen::Vector3d> &&entryPoints,
                   std::vector<Eigen::Vector3d> &&exitPoints,
                   std::vector<double> &&pathLengths,
                   std::vector<size_t> &&pathComponentIndexes,
                   std::vector<size_t> &&detectorComponentIndexes,
                   std::vector<DetectorIdType> &&detectorIds,
                   size_t sourceIndex, size_t sampleIndex)
    : m_proxies(std::move(proxies)), m_positions(std::move(positions)),
      m_rotations(std::move(rotations)),
      m_componentIds(std::move(componentIds)),
      m_entryPoints(std::move(entryPoints)),
      m_exitPoints(std::move(exitPoints)),
      m_pathLengths(std::move(pathLengths)),
      m_pathComponentIndexes(std::move(pathComponentIndexes)),
      m_detectorComponentIndexes(std::move(detectorComponentIndexes)),
      m_detectorIds(std::move(detectorIds)), m_sourceIndex(sourceIndex),
      m_sampleIndex(sampleIndex) {
  // Note that m_rootComponent is not set because we don't have one.
}

const ComponentProxy &FlatTree::rootProxy() const { return m_proxies[0]; }

void FlatTree::fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const {

  const size_t nEntries = m_detectorIds.size();
  for (size_t i = 0; i < nEntries; ++i) {
    toFill.insert(std::make_pair(m_detectorIds[i], i));
  }
}

void
FlatTree::fillComponentMap(std::map<ComponentIdType, size_t> &toFill) const {
  const size_t nEntries = m_componentIds.size();
  for (size_t i = 0; i < nEntries; ++i) {
    toFill.insert(std::make_pair(m_componentIds[i], i));
  }
}

size_t FlatTree::sampleComponentIndex() const {
  return pathIndexToCompIndex(m_sampleIndex);
}

size_t FlatTree::sourceComponentIndex() const {
  return pathIndexToCompIndex(m_sourceIndex);
}

size_t FlatTree::samplePathIndex() const { return m_sampleIndex; }

size_t FlatTree::sourcePathIndex() const { return m_sourceIndex; }

size_t FlatTree::componentSize() const { return m_proxies.size(); }

std::vector<size_t> FlatTree::subTreeIndexes(size_t proxyIndex) const {
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

std::vector<Eigen::Vector3d> FlatTree::startPositions() const {
  return m_positions;
}

std::vector<Eigen::Quaterniond> FlatTree::startRotations() const {
  return m_rotations;
}

std::vector<Eigen::Vector3d> FlatTree::startExitPoints() const {
  return m_exitPoints;
}

std::vector<Eigen::Vector3d> FlatTree::startEntryPoints() const {
  return m_entryPoints;
}

std::vector<double> FlatTree::pathLengths() const { return m_pathLengths; }

std::vector<size_t> FlatTree::detectorComponentIndexes() const {
  return m_detectorComponentIndexes;
}

size_t FlatTree::detIndexToCompIndex(size_t detectorIndex) const {
  return m_detectorComponentIndexes[detectorIndex];
}

size_t FlatTree::pathIndexToCompIndex(size_t pathIndex) const {
  return m_pathComponentIndexes[pathIndex];
}

std::shared_ptr<Component> FlatTree::rootComponent() const {
  return m_componentRoot;
}

size_t FlatTree::nDetectors() const {
  return m_detectorComponentIndexes.size();
}

size_t FlatTree::nPathComponents() const {
  return m_pathComponentIndexes.size();
}

const ComponentProxy &FlatTree::proxyAt(size_t index) const {
  return m_proxies[index];
}

std::vector<ComponentProxy>::const_iterator FlatTree::begin() const {
  return m_proxies.begin();
}
std::vector<ComponentProxy>::const_iterator FlatTree::end() const {
  return m_proxies.end();
}
std::vector<ComponentProxy>::const_iterator FlatTree::cbegin() const {
  return m_proxies.cbegin();
}
std::vector<ComponentProxy>::const_iterator FlatTree::cend() const {
  return m_proxies.cend();
}

bool FlatTree::operator==(const FlatTree &other) const {
  // We only need to compare the proxies.
  return m_proxies == other.m_proxies;
}

bool FlatTree::operator!=(const FlatTree &other) const {
  return !operator==(other);
}
