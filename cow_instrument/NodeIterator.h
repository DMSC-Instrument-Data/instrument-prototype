#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H


#include <stack>
#include <memory>

class Node;
class NodeIterator {

public:

   NodeIterator(std::unique_ptr<const Node>&& begin);
   NodeIterator(const Node& begin);
   Node const * const next();
   bool atEnd() const;

private:
   /// Node iterator owns the thing it iterates over.
   std::unique_ptr<const Node> m_iterateable;
   void addToBuffer(Node const * const item);
   std::stack<Node const * > m_buffer;

};

#endif
