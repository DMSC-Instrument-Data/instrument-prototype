#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>

class Node;
class NodeIterator;

/*
 The instrument is nothing more than syntatic sugar over the root Node.
 */
class InstrumentTree {
public:

    InstrumentTree(std::shared_ptr<const Node> root);

    std::unique_ptr<NodeIterator> iterator() const;

    std::shared_ptr<const Node> root() const;

private:

    std::shared_ptr<const Node> m_root;

};

#endif
