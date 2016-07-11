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
 * Derived, detector vector is always recreated. Except on a plain-old copy.
*/
void InstrumentTree::init(size_t nDetectors) {
  // This will make the push_backs faster
  m_detectorVec->reserve(nDetectors);

  if (m_nodes->empty()) {
    throw std::invalid_argument(
        "No root Node. Cannot create an InstrumentTree");
  }

  const unsigned int expectedVersion = this->version();
  for (const auto &node : m_nodes.const_ref()) {
    const auto &component = node.const_ref();
    // Put all detectors into a flat map.
    findDetectors(component, *m_detectorVec);
    if (node.version() != expectedVersion) {
      throw std::invalid_argument(
          "Cannot make an Instrument tree around Nodes of differing version");
    }
  }
  // Should we shrink to fit to reduce excess capacity?
  // m_detectorVec.shrink_to_fit(); This could be costly
}

InstrumentTree::InstrumentTree(std::vector<Node> &&nodes, size_t nDetectors)
    : m_nodes(std::make_shared<std::vector<Node>>(std::move(nodes))),
      m_detectorVec(std::make_shared<std::vector<Detector const *>>()) {

  init(nDetectors);
}

InstrumentTree::InstrumentTree(CowPtr<std::vector<Node>> nodes,
                               size_t nDetectors)
    : m_nodes(nodes),
      m_detectorVec(std::make_shared<std::vector<Detector const *>>()) {

  init(nDetectors);
}

const Node &InstrumentTree::root() const { return m_nodes.const_ref()[0]; }

const Detector &InstrumentTree::getDetector(size_t detectorIndex) const {

  if (detectorIndex >= m_detectorVec->size()) {
    throw std::invalid_argument(
        "Index is outside range of detector ids. Index is: " +
        std::to_string(detectorIndex));
  }

  return *m_detectorVec.const_ref()[detectorIndex];
}

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

V3D InstrumentTree::sourcePos() const {
  return V3D{0, 0, 0}; // HACK!
}

V3D InstrumentTree::samplePos() const {
  return V3D{0, 0, 10}; // HACK!
}

size_t InstrumentTree::nDetectors() const { return m_detectorVec->size(); }

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
    init(nDetectors);
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
