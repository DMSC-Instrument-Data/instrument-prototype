#include "NodeIterator.h"
#include "Node.h"

NodeIterator::NodeIterator(Node_const_sptr begin) : m_iterateable(begin) {
  /*
There's probably a better way to do this. The current approach of flattening
the tree aprori is inferior to calcuating on the fly. However this is not a
binary tree, and c++ has no std yield.
*/
  addToBuffer(m_iterateable.get());
}

void NodeIterator::addToBuffer(Node const *const item) {

  m_buffer.push(item);
  for (size_t i = 0; i < item->nChildren(); ++i) {
    addToBuffer(&item->child(i));
  }
}

Node const * const NodeIterator::next() {
  if (!atEnd()) {
    auto current = m_buffer.top();
    m_buffer.pop();
    return current;
  } else {
    return nullptr;
  }
}

bool NodeIterator::atEnd() const { return m_buffer.empty(); }
