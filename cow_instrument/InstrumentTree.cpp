#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "NodeIterator.h"
#include <utility>
#include <string>
#include <mutex>

namespace {

void findDetectors(const Component &component,
                   std::map<size_t, const Detector *> &store) {
  if (Detector const *detector = dynamic_cast<const Detector *>(&component)) {
    store.insert(std::make_pair(detector->id(), detector));
  } else if (CompositeComponent const *composite =
                 dynamic_cast<const CompositeComponent *>(&component)) {
    for (size_t i = 0; i < composite->size(); ++i) {
      findDetectors(*composite->getChild(i), store);
    }
  }
}
}

InstrumentTree::InstrumentTree(Node_const_sptr root) : m_root(root) {}

InstrumentTree::InstrumentTree(const InstrumentTree &other)
    : m_detectorMap(other.m_detectorMap), m_root(other.m_root) {}

InstrumentTree &InstrumentTree::operator=(const InstrumentTree &other) {
  if (this != &other) {
    m_detectorMap = other.m_detectorMap;
    m_root = other.m_root;
  }
  return *this;
}

std::unique_ptr<NodeIterator> InstrumentTree::iterator() const {
  return std::unique_ptr<NodeIterator>(new NodeIterator(m_root));
}

Node_const_sptr InstrumentTree::root() const { return m_root; }

void InstrumentTree::init() const {

  auto it = this->iterator();
  while (!it->atEnd()) {
    // Not pretty
    const auto &component = it->next()->const_ref();
    findDetectors(component, m_detectorMap);
  }
}

const Detector &InstrumentTree::getDetector(size_t detectorId) const {

  std::call_once(m_detectorMapFlag, &InstrumentTree::init, this);

  auto it = m_detectorMap.find(detectorId);
  if (it == m_detectorMap.end()) {
    throw std::invalid_argument("No detector by this id " +
                                std::to_string(detectorId));
  }
  return *(it->second);
}

