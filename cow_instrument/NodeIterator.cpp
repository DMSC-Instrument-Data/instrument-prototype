#include "NodeIterator.h"
#include "Node.h"

NodeIterator::NodeIterator(Node_const_uptr &&begin)
    : m_iterateable(std::move(begin)) {
  addToBuffer(m_iterateable.get());
}

NodeIterator::NodeIterator(const Node &begin) : m_iterateable(begin.clone()) {}

void NodeIterator::addToBuffer(Node const *const item) {

  m_buffer.push(item);
  for (size_t i = 0; i < item->nChildren(); ++i) {
    addToBuffer(&item->child(i));
  }
}

Node const *const NodeIterator::next() {
  if (!atEnd()) {
    auto current = m_buffer.top();
    m_buffer.pop();
    return current;
  } else {
    return nullptr;
  }
}

bool NodeIterator::atEnd() const { return m_buffer.empty(); }
