#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "PathComponent.h"
#include "Command.h"
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
void findKeyComponents(const Component &component,
                       std::vector<const Detector *> &detectorStore,
                       std::vector<const PathComponent *> &pathStore) {

  // Walk through and register all detectors on the store.
  component.registerContents(detectorStore, pathStore);
}

void checkDetectorRange(size_t detectorIndex,
                        const std::vector<Detector const *> &detectorVec) {
  if (detectorIndex >= detectorVec.size()) {
    throw std::invalid_argument(
        "Detector Index is outside range of indexes. Index is: " +
        std::to_string(detectorIndex));
  }
}

void checkPathRange(size_t pathIndex,
                    const std::vector<PathComponent const *> &pathVec) {
  if (pathIndex >= pathVec.size()) {
    throw std::invalid_argument(
        "PathComponent Index is outside range of indexes. Index is: " +
        std::to_string(pathIndex));
  }
}

bool findSource(const PathComponent *item) { return item->isSource(); }

bool findSample(const PathComponent *item) { return item->isSample(); }
}

void InstrumentTree::init() {

  if (m_nodes->empty()) {
    throw std::invalid_argument(
        "No root Node. Cannot create an InstrumentTree");
  }

  const unsigned int expectedVersion = this->version();
  for (const auto &node : m_nodes.const_ref()) {
    const auto &component = node.const_ref();
    // Put all detectors into a flat map.
    findKeyComponents(component, *m_detectorVec, *m_pathVec);
    if (node.version() != expectedVersion) {
      throw std::invalid_argument(
          "Cannot make an Instrument tree around Nodes of differing version");
    }
  }
  const auto begin_pathVec = m_pathVec.const_ref().cbegin();
  const auto end_pathVec = m_pathVec.const_ref().cend();
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

InstrumentTree::InstrumentTree(std::vector<Node> &&nodes)
    : m_nodes(std::make_shared<std::vector<Node>>(std::move(nodes))),
      m_detectorVec(std::make_shared<std::vector<Detector const *>>()),
      m_pathVec(std::make_shared<std::vector<PathComponent const *>>()) {

  init();
}

InstrumentTree::InstrumentTree(CowPtr<std::vector<Node>> nodes)
    : m_nodes(nodes),
      m_detectorVec(std::make_shared<std::vector<Detector const *>>()),
      m_pathVec(std::make_shared<std::vector<PathComponent const *>>()) {

  init();
}

const Node &InstrumentTree::root() const { return m_nodes.const_ref()[0]; }

const Detector &InstrumentTree::getDetector(size_t detectorIndex) const {

  checkDetectorRange(detectorIndex, m_detectorVec.const_ref());

  return *m_detectorVec.const_ref()[detectorIndex];
}

const PathComponent &
InstrumentTree::getPathComponent(size_t pathComponentIndex) const {

  checkPathRange(pathComponentIndex, m_pathVec.const_ref());

  return *m_pathVec.const_ref()[pathComponentIndex];
};

unsigned int InstrumentTree::version() const {
  return m_nodes.const_ref()[0].version();
}

void InstrumentTree::fillDetectorMap(
    std::map<DetectorIdType, size_t> &toFill) const {
  for (size_t index = 0; index < m_detectorVec->size(); ++index) {
    toFill.insert(
        std::make_pair(m_detectorVec.const_ref()[index]->detectorId(), index));
  }
}

const PathComponent &InstrumentTree::source() const {
  return *m_pathVec.const_ref()[m_sourceIndex];
}

const PathComponent &InstrumentTree::sample() const {
  return *m_pathVec.const_ref()[m_sampleIndex];
}

size_t InstrumentTree::samplePathIndex() const { return m_sampleIndex; }

size_t InstrumentTree::sourcePathIndex() const { return m_sourceIndex; }

size_t InstrumentTree::nDetectors() const { return m_detectorVec->size(); }

size_t InstrumentTree::nPathComponents() const { return m_pathVec->size(); }

bool InstrumentTree::modify(size_t nodeIndex, const Command &command) {

  /* We increment the version number of the Nodes. Consider removing the Node
   * Version Number. It has no practicle use-case other than diagnostics, and
   * the increemnt
   * is a writeable operation on the COW ptr.
   */
  std::for_each(m_nodes->begin(), m_nodes->end(),
                [](Node &node) { node.incrementVersion(); });

  bool newDetectors = false;
  if (command.isMetaDataCommand()) {
    // No cascading behaviour.
    auto &currentNode = m_nodes->operator[](nodeIndex);
    newDetectors |= currentNode.modify(command);

  } else {
    // Cascading behaviour
    std::vector<size_t> toModify = {nodeIndex};
    for (size_t index = 0; index < toModify.size(); ++index) {
      auto &currentNode = m_nodes->operator[](toModify[index]);
      newDetectors |= currentNode.modify(command);
      const auto &currentChildren = currentNode.children();
      toModify.insert(toModify.end(), currentChildren.begin(),
                      currentChildren.end());
    }
  }

  /*
   * We only rebuild our Detector* vector if the modifications
   * indicate that such action is required.
   */
  if (newDetectors) {
    const size_t nDetectors = m_detectorVec->size();
    m_detectorVec->clear();
    init();
  }
  return newDetectors;
}

bool InstrumentTree::modify(const Node *node, const Command &command) {
  for (size_t index = 0; index < m_nodes->size(); ++index) {
    if (&m_nodes.const_ref()[index] == node) {
      return modify(index, command);
    }
  }
  throw std::invalid_argument("Node has not been found");
}

const Node *const InstrumentTree::nodeAt(size_t index) const {
  return &m_nodes.const_ref()[index];
}
