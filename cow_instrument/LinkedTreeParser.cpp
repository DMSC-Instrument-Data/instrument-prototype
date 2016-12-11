
#include "CompositeComponent.h"
#include "Detector.h"
#include "LinkedTreeParser.h"
#include "PathComponent.h"
#include <string>
#include <algorithm>
#include <iterator>

void LinkedTreeParser::registerDetector(Detector const *const comp) {

  const size_t newIndex = coreUpdate(comp);
  m_detectorComponentIndexes.push_back(newIndex);
}

void LinkedTreeParser::registerPathComponent(PathComponent const *const comp) {

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

size_t
LinkedTreeParser::registerComposite(CompositeComponent const *const comp) {
  return coreUpdate(comp);
}

void LinkedTreeParser::registerDetector(const Detector *const comp,
                                        size_t parentIndex) {
  const size_t newIndex = coreUpdate(comp, parentIndex);
  m_detectorComponentIndexes.push_back(newIndex);
  m_detectorIds.push_back(comp->detectorId());
}

void LinkedTreeParser::registerPathComponent(const PathComponent *const comp,
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

size_t LinkedTreeParser::registerComposite(const CompositeComponent *const comp,
                                           size_t parentIndex) {
  return coreUpdate(comp, parentIndex);
}

std::vector<ComponentProxy> LinkedTreeParser::proxies() { return m_proxies; }

size_t LinkedTreeParser::componentSize() const { return m_proxies.size(); }

size_t LinkedTreeParser::detectorSize() const {
  return m_detectorComponentIndexes.size();
}

size_t LinkedTreeParser::pathSize() const {
  return m_pathComponentIndexes.size();
}

std::vector<size_t> LinkedTreeParser::pathComponentIndexes() const {
  return m_pathComponentIndexes;
}

std::vector<size_t> LinkedTreeParser::detectorComponentIndexes() const {
  return m_detectorComponentIndexes;
}

std::vector<Eigen::Vector3d> LinkedTreeParser::startEntryPoints() const {
  return m_entryPoints;
}

std::vector<Eigen::Vector3d> LinkedTreeParser::startExitPoints() const {
  return m_exitPoints;
}

std::vector<double> LinkedTreeParser::pathLengths() const {
  return m_pathLengths;
}

size_t LinkedTreeParser::coreUpdate(Component const *const comp,
                                    size_t previousIndex) {
  size_t newIndex = m_proxies.size();
  m_componentIds.emplace_back(comp->componentId());
  m_proxies.emplace_back(previousIndex, comp->componentId());
  m_proxies[previousIndex].addChild(newIndex);
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

size_t LinkedTreeParser::coreUpdate(Component const *const comp) {
  const size_t newIndex = m_proxies.size();
  m_componentIds.emplace_back(comp->componentId());
  m_proxies.emplace_back(comp->componentId());
  m_positions.emplace_back(comp->getPos());
  m_rotations.emplace_back(comp->getRotation());
  return newIndex; // Return the last index.
}

std::vector<Eigen::Vector3d> LinkedTreeParser::startPositions() const {
  return m_positions;
}

std::vector<Eigen::Quaterniond> LinkedTreeParser::startRotations() const {
  return m_rotations;
}

std::vector<ComponentIdType> LinkedTreeParser::componentIds() const {
  return m_componentIds;
}

std::vector<DetectorIdType> LinkedTreeParser::detectorIds() const {
  return m_detectorIds;
}

int64_t LinkedTreeParser::sourcePathIndex() const { return m_sourceIndex; }

int64_t LinkedTreeParser::samplePathIndex() const { return m_sampleIndex; }
