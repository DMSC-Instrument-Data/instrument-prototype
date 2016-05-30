#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "Command.h"
#include <utility>
#include <string>
#include <algorithm>

namespace {

void findDetectors(const Component &component,
                   std::vector<const Detector *> &store) {

  // Walk through and register all detectors on the store.
  component.registerContents(store);
}
}

/*
 * Derived, detector vector is always recreated.
*/
InstrumentTree::InstrumentTree(std::vector<Node> &&nodes, size_t nDetectors)
    : m_nodes(std::move(nodes)) {

  // This will make the push_backs faster
  m_detectorVec.reserve(nDetectors);

  if (m_nodes.empty()) {
    throw std::invalid_argument(
        "No root Node. Cannot create an InstrumentTree");
  }

  const unsigned int expectedVersion = this->version();
  for (const auto &node : m_nodes) {
    const auto &component = node.const_ref();
    // Put all detectors into a flat map.
    findDetectors(component, m_detectorVec);
    if (node.version() != expectedVersion) {
      throw std::invalid_argument(
          "Cannot make an Instrument tree around Nodes of differing version");
    }
  }

  // Should we shrink to fit to reduce excess capacity?
  // m_detectorVec.shrink_to_fit(); This could be costly
}

const Node &InstrumentTree::root() const { return m_nodes[0]; }

const Detector &InstrumentTree::getDetector(size_t detectorIndex) const {

  if (detectorIndex >= m_detectorVec.size()) {
    throw std::invalid_argument(
        "Index is outside range of detector ids. Index is: " +
        std::to_string(detectorIndex));
  }

  return *m_detectorVec[detectorIndex];
}

unsigned int InstrumentTree::version() const { return m_nodes[0].version(); }

void InstrumentTree::fillDetectorMap(
    std::map<DetectorIdType, size_t> &toFill) const {
  for (size_t index = 0; index < m_detectorVec.size(); ++index) {
    toFill.insert(std::make_pair(m_detectorVec[index]->detectorId(), index));
  }
}

V3D InstrumentTree::sourcePos() const {

  return V3D{0, 0, 0}; // HACK!
}



size_t InstrumentTree::nDetectors() const { return m_detectorVec.size(); }

std::unique_ptr<const InstrumentTree>
InstrumentTree::modify(size_t node, const Command &command) const {

  auto newNodes(m_nodes);
  std::for_each(newNodes.begin(), newNodes.end(),
                [](Node &node) { node.incrementVersion(); });

  if (command.isMetaDataCommand()) {
    // No cascading behaviour.
    auto &currentNode = newNodes[node];
    currentNode.doModify(command);
  } else {
    // Cascading behaviour
      std::vector<size_t> toModify = {node};
    for (size_t index = 0; index < toModify.size(); ++index) {
      auto &currentNode = newNodes[toModify[index]];
      currentNode.doModify(command);
      const auto &currentChildren = currentNode.children();
      toModify.insert(toModify.end(), currentChildren.begin(),
                      currentChildren.end());
    }
  }
  return std::unique_ptr<const InstrumentTree>(
      new InstrumentTree(std::move(newNodes), m_detectorVec.size()));
}

std::unique_ptr<const InstrumentTree>
InstrumentTree::modify(const Node *node, const Command &command) const {
  for (size_t index = 0; index < m_nodes.size(); ++index) {
    if (&m_nodes[index] == node) {
      return modify(index, command);
    }
  }
  throw std::invalid_argument("Node has not been found");
}

const Node *const InstrumentTree::nodeAt(size_t index) const {
  return &m_nodes[index];
}
