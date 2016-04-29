#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>
#include <vector>
#include <map>
#include "IdType.h"


class Node;
class NodeIterator;
class Command;
class Detector;

/*
 The instrument is nothing more than syntatic sugar over the root Node.
 */
class InstrumentTree {
public:
  InstrumentTree(std::vector<Node> &&nodes, size_t nDetectors);

  const Node &root() const;

  const Detector &getDetector(size_t detectorIndex) const;

  unsigned int version() const;

  InstrumentTree modify(size_t node, const Command &command) const;
  InstrumentTree modify(const Node *node, const Command &command) const;

  // This is how we help the client out when they want to work with detector ids.
  void fillDetectorMap(const std::map<DetectorIdType, size_t>& toFill);

  size_t nDetectors() const;

  void addChild(const Node *node, const Command &command) {
    // find node index
    // append new node to m_nodes, parent = index
    // parent.addChild(m_nodes.size()-1)
  }

  std::vector<Node>::const_iterator begin() const { return m_nodes.begin(); }
  std::vector<Node>::const_iterator end() const { return m_nodes.end(); }
  std::vector<Node>::const_iterator cbegin() const { return m_nodes.cbegin(); }
  std::vector<Node>::const_iterator cend() const { return m_nodes.cend(); }

private:
  std::vector<Detector const*> m_detectorVec;

  std::vector<Node> m_nodes;

public:
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;

#endif
