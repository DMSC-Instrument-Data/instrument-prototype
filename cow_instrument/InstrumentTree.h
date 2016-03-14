#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>
#include <map>

class Node;
class NodeIterator;
class Detector;

/*
 The instrument is nothing more than syntatic sugar over the root Node.
 */
class InstrumentTree {
public:

    InstrumentTree(std::shared_ptr<const Node> root);

    std::unique_ptr<NodeIterator> iterator() const;

    std::shared_ptr<const Node> root() const;

    const Detector& getDetector(size_t detectorId) const;

private:

    std::map<size_t, Detector const *> m_detectorMap;

    std::shared_ptr<const Node> m_root;

};

#endif
