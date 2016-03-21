#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "NodeIterator.h"
#include <utility>
#include <string>

namespace {

void findDetectors(const Component &component,
                   std::map<size_t, const Detector *> &store) {

  // Walk through and register all detectors on the store.
  component.registerDetectors(store);
}
}

InstrumentTree::InstrumentTree(Node_const_uptr&& root) : m_root(std::move(root)) {

  if(!m_root){
      throw std::invalid_argument("No root Node. Cannot create an InstrumentTree");
  }

  const unsigned int expectedVersion = this->version();
  auto it = this->iterator();
  while (!it->atEnd()) {
    auto node = it->next();
    const auto &component = node->const_ref();
    // Put all detectors into a flat map.
    findDetectors(component, m_detectorMap);
    if (node->version() != expectedVersion) {
      throw std::invalid_argument(
          "Cannot make an Instrument tree around Nodes of differing version");
    }
  }
}

std::unique_ptr<NodeIterator> InstrumentTree::iterator() const {
  return std::unique_ptr<NodeIterator>(new NodeIterator(m_root->clone()));
}

const Node& InstrumentTree::root() const { return *m_root; }

const Detector &InstrumentTree::getDetector(size_t detectorId) const {

  auto it = m_detectorMap.find(detectorId);
  if (it == m_detectorMap.end()) {
    throw std::invalid_argument("No detector by this id " +
                                std::to_string(detectorId));
  }
  return *(it->second);
}

unsigned int InstrumentTree::version() const { return m_root->version(); }
