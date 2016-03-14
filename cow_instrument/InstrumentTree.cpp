#include "InstrumentTree.h"
#include "Node.h"
#include "Detector.h"
#include "CompositeComponent.h"
#include "NodeIterator.h"
#include <utility>
#include <string>

void InstrumentTree::findDetectors(const Component& component)
{

    if (Detector const *detector = dynamic_cast<const Detector *>(&component)) {
      m_detectorMap.insert(std::make_pair(detector->id(), detector));
    } else if (CompositeComponent const *composite =
                   dynamic_cast<const CompositeComponent *>(&component)) {
      for(size_t i = 0; i < composite->size(); ++i){
          findDetectors(*composite->getChild(i));
      }
    }
}

InstrumentTree::InstrumentTree(Node_const_sptr root) : m_root(root) {



  // TODO. Maybe we don't always want to do this?
  auto it = this->iterator();
  while (!it->atEnd()) {
    // Not pretty
    const auto &component = it->next()->const_ref();
    findDetectors(component);
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
