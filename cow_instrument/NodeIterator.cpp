#include "NodeIterator.h"
#include "Node.h"

NodeIterator::NodeIterator(Node_const_sptr begin) {
  /*
There's probably a better way to do this. The current approach of flattening
the tree aprori is inferior to calcuating on the fly. However this is not a
binary tree, and c++ has no std yield.
*/
  addToBuffer(begin);
}

void NodeIterator::addToBuffer(Node_const_sptr item) {

  m_buffer.push(item);
  auto children = item->children();
  for (auto it = children.begin(); it != children.end(); ++it)
    addToBuffer(*it);
}

Node_const_sptr NodeIterator::next() {
  if (!atEnd()) {
    auto current = m_buffer.top();
    m_buffer.pop();
    return current;
  } else {
    return Node_const_sptr(nullptr);
  }
}


bool NodeIterator::atEnd() const
{
    return m_buffer.empty();
}
