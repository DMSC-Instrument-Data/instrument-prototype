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


InstrumentTree::InstrumentTree(Node_const_sptr root) : m_root(root) {

  // TODO. Maybe we don't always want to do this?
  auto it = this->iterator();
  while (!it->atEnd()) {
    const auto &component = it->next()->const_ref();
    findDetectors(component, m_detectorMap);
  }
}

std::unique_ptr<NodeIterator> InstrumentTree::iterator() const {
  return std::unique_ptr<NodeIterator>(new NodeIterator(m_root));
}

Node_const_sptr InstrumentTree::root() const { return m_root; }

const Detector &InstrumentTree::getDetector(size_t detectorId) const {

  auto it = m_detectorMap.find(detectorId);
  if (it == m_detectorMap.end()) {
    throw std::invalid_argument("No detector by this id " +
                                std::to_string(detectorId));
  }
  return *(it->second);
}


