#ifndef NODE_ITERATOR_H
#define NODE_ITERATOR_H


#include <stack>
#include <memory>

class Node;
class NodeIterator {

public:

   NodeIterator(std::shared_ptr<const Node> begin);
   std::shared_ptr<const Node> next();
   bool atEnd() const;

private:
   void addToBuffer(std::shared_ptr<const Node> item);
   std::stack<std::shared_ptr<const Node> > m_buffer;

};

#endif
