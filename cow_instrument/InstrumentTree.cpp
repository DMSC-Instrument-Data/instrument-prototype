#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "NodeIterator.h"
#include <utility>
#include <string>

namespace {

void findDetectors(const Component &component,
                   std::vector<const Detector *> &store) {

  // Walk through and register all detectors on the store.
  component.registerContents(store);
}
}

InstrumentTree::InstrumentTree(Node_const_uptr &&root, size_t nDetectors)
    : m_root(std::move(root)) {

  // This will make the push_backs faster
  m_detectorVec.reserve(nDetectors);

  if (!m_root) {
    throw std::invalid_argument(
        "No root Node. Cannot create an InstrumentTree");
  }

  const unsigned int expectedVersion = this->version();
  auto it = this->iterator();
  while (!it->atEnd()) {
    auto node = it->next();
    const auto &component = node->const_ref();
    // Put all detectors into a flat map.
    findDetectors(component, m_detectorVec);
    if (node->version() != expectedVersion) {
      throw std::invalid_argument(
          "Cannot make an Instrument tree around Nodes of differing version");
    }
  }

  // Should we shrink to fit to reduce excess capacity?
  // m_detectorVec.shrink_to_fit(); This could be costly
}

InstrumentTree::InstrumentTree(const InstrumentTree &other)
    : InstrumentTree(other.root().clone(), other.m_detectorVec.size()) {}

std::unique_ptr<NodeIterator> InstrumentTree::iterator() const {
  return std::unique_ptr<NodeIterator>(new NodeIterator(m_root->clone()));
}

const Node &InstrumentTree::root() const { return *m_root; }

const Detector &InstrumentTree::getDetector(size_t detectorIndex) const {

  if (detectorIndex >= m_detectorVec.size()) {
    throw std::invalid_argument(
        "Index is outside range of detector ids. Index is: " +
        std::to_string(detectorIndex));
  }

  return *m_detectorVec[detectorIndex];
}

unsigned int InstrumentTree::version() const { return m_root->version(); }

void InstrumentTree::fillDetectorMap(
    const std::map<DetectorIdType, size_t> &) const {
  throw std::runtime_error("Not Implemented. But likely required.");
  /*
   * Should be simple enough. We have the vector already, just interogate that
   * to create the map.
  */
}

size_t InstrumentTree::nDetectors() const { return m_detectorVec.size(); }

V3D InstrumentTree::sourcePos() const {

  return V3D{0, 0, 0}; // HACK!
}

V3D InstrumentTree::samplePos() const {

  return V3D{0, 0, 20}; // HACK!
}

InstrumentTree_const_uptr InstrumentTree::modify(const Command &command) const {
  return InstrumentTree_const_uptr(
      new InstrumentTree(m_root->modify(command), m_detectorVec.size()));
}
