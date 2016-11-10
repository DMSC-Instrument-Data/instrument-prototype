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
void findKeyComponents(const Component &component, ComponentInfo &info) {

  // Walk through and register all detectors on the store.
  component.registerContents(info);
}

bool findSource(const PathComponent *item) { return item->isSource(); }

bool findSample(const PathComponent *item) { return item->isSample(); }
}

void InstrumentTree::init() {

  if (m_nodes->size() > 0) {
  const unsigned int expectedVersion = this->version();
  for (const auto &node : m_nodes.const_ref()) {
    const auto &component = node.const_ref();
    // Put all detectors into a flat map.
    // std::vector<ComponentProxy> m_componentProxies;
    findKeyComponents(component, m_componentInfo);
    if (node.version() != expectedVersion) {
      throw std::invalid_argument(
          "Cannot make an Instrument tree around Nodes of differing version");
    }
  }
  } else {
    findKeyComponents(*m_componentRoot, m_componentInfo);
  }

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
    : m_nodes(std::make_shared<std::vector<Node>>()),
      m_componentRoot(componentRoot) {
  init();
}

InstrumentTree::InstrumentTree(std::vector<Node> &&nodes)
    : m_nodes(std::make_shared<std::vector<Node>>(std::move(nodes))) {

  init();
}

InstrumentTree::InstrumentTree(CowPtr<std::vector<Node>> nodes)
    : m_nodes(nodes) {

  init();
}

const Node &InstrumentTree::root() const { return m_nodes.const_ref()[0]; }

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

unsigned int InstrumentTree::version() const {
  return m_nodes.const_ref()[0].version();
}

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

CowPtr<std::vector<Node>>::RefPtr InstrumentTree::unsafeContents() const {
  return m_nodes.heldValue();
}

size_t InstrumentTree::nodeSize() const { return m_nodes->size(); }

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

size_t InstrumentTree::nDetectors() const {
  return m_componentInfo.detectorSize();
}

size_t InstrumentTree::nPathComponents() const {
  return m_componentInfo.pathSize();
}

/*
 * Modify is not thread-safe owing to the fact that the vector of detector
 * and vector of path component pointers is potentially corruptable between
 * the time that the cow occurs in the Command::Modify and the
 * InstrumentTree::init().
 */
bool InstrumentTree::modify(size_t nodeIndex, const Command &command) {

  /* We increment the version number of the Nodes. Consider removing the Node
   * Version Number. It has no practicle use-case other than diagnostics, and
   * the increemnt
   * is a writeable operation on the COW ptr.
   */
  std::for_each(m_nodes->begin(), m_nodes->end(),
                [](Node &node) { node.incrementVersion(); });

  bool cowTriggered = false;
  if (command.isMetaDataCommand()) {
    // No cascading behaviour.
    auto &currentNode = m_nodes->operator[](nodeIndex);
    cowTriggered |= currentNode.modify(command);

  } else {
    // Cascading behaviour
    std::vector<size_t> toModify = {nodeIndex};
    for (size_t index = 0; index < toModify.size(); ++index) {
      auto &currentNode = m_nodes->operator[](toModify[index]);
      cowTriggered |= currentNode.modify(command);
      const auto &currentChildren = currentNode.children();
      toModify.insert(toModify.end(), currentChildren.begin(),
                      currentChildren.end());
    }
  }

  /*
   * We only rebuild our Detector* vector if the modifications
   * indicate that such action is required.
   */
  if (cowTriggered) {
    // We must purge any existing memory.
    m_componentInfo.clear();
    init();
  }
  return cowTriggered;
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
