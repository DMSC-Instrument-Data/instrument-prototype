#include "InstrumentTree.h"
#include "Node.h"
#include "NodeIterator.h"

InstrumentTree::InstrumentTree(Node_const_sptr root) : m_root(root)
{
}

std::unique_ptr<NodeIterator> InstrumentTree::iterator() const{
    return std::unique_ptr<NodeIterator>(new NodeIterator(m_root));
}

Node_const_sptr InstrumentTree::root() const{
    return m_root;
}
