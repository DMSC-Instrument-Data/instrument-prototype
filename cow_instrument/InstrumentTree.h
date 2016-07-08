#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>
#include <vector>
#include <map>
#include "cow_ptr.h"
#include "IdType.h"
#include "V3D.h"

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

  InstrumentTree(CowPtr<std::vector<Node>> nodes, size_t nDetectors);

  const Node &root() const;

  const Detector &getDetector(size_t detectorIndex) const;

  unsigned int version() const;

  bool modify(size_t node, const Command &command);
  bool modify(const Node *node, const Command &command);

  // This is how we help the client out when they want to work with detector
  // ids.
  void fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const;

  size_t nDetectors() const;

  Node const *const nodeAt(size_t index) const;
  V3D sourcePos() const;

  std::vector<Node>::const_iterator begin() const {
    return m_nodes.const_ref().begin();
  }
  std::vector<Node>::const_iterator end() const {
    return m_nodes.const_ref().end();
  }
  std::vector<Node>::const_iterator cbegin() const {
    return m_nodes.const_ref().cbegin();
  }
  std::vector<Node>::const_iterator cend() const {
    return m_nodes.const_ref().cend();
  }
  V3D samplePos() const;

private:
  void init(size_t nDetectors);

  CowPtr<std::vector<Detector const *>> m_detectorVec;
  CowPtr<std::vector<Node>> m_nodes;
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;
using InstrumentTree_uptr = std::unique_ptr<const InstrumentTree>;

#endif
