#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>
#include <vector>
#include "Detector.h"

class Node;
class NodeIterator;
class Command;

/*
 The instrument is nothing more than syntatic sugar over the root Node.
 */
class InstrumentTree {
public:
  InstrumentTree(std::unique_ptr<const Node> &&root, size_t nDetectors);

  std::unique_ptr<NodeIterator> iterator() const;

  const Node &root() const;

  const Detector &getDetector(size_t detectorIndex) const;

  unsigned int version() const;

  std::unique_ptr<const InstrumentTree> modify(const Command &command) const;

private:

  std::vector<Detector const*> m_detectorVec;

  std::unique_ptr<const Node> m_root;
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;

#endif
