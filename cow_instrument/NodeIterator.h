#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H


#include <stack>
#include <memory>

class Node;
class NodeIterator {

public:

   NodeIterator(std::shared_ptr<const Node> begin);
   Node const * const next();
   bool atEnd() const;

private:
   // To guarantee we have some ownership of the thing we are iterating. TODO!
   std::shared_ptr<const Node> m_iterateable;
   void addToBuffer(Node const * const item);
   std::stack<Node const * const > m_buffer;

};

#endif
