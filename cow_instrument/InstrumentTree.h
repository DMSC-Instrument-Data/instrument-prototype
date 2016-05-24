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
  InstrumentTree(std::unique_ptr<const Node> &&root, size_t nDetectors);

  InstrumentTree(const InstrumentTree& other);

  std::unique_ptr<NodeIterator> iterator() const;

  const Node &root() const;

  const Detector &getDetector(size_t detectorIndex) const;

  unsigned int version() const;

  std::unique_ptr<const InstrumentTree> modify(const Command &command) const;

  // This is how we help the client out when they want to work with detector ids.
  void fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const;

  size_t nDetectors() const;

  V3D sourcePos() const;

  V3D samplePos() const;

private:

  //const std::shared_ptr<std::map<DetectorIdType, size_t> > m_detectorIdMap;

  std::vector<Detector const*> m_detectorVec;

  std::unique_ptr<const Node> m_root;
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;

#endif
