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
class PathComponent;
class PointSource;
class PointSample;

/*
 The instrument is nothing more than syntatic sugar over the root Node.
 */
class InstrumentTree {
public:
  InstrumentTree(std::vector<Node> &&nodes);

  InstrumentTree(CowPtr<std::vector<Node>> nodes);

  const Node &root() const;

  const Detector &getDetector(size_t detectorIndex) const;

  const PathComponent &getPathComponent(size_t pathComponentIndex) const;
  const PathComponent &source() const;
  const PathComponent &sample() const;
  unsigned int version() const;

  bool modify(size_t node, const Command &command);
  bool modify(const Node *node, const Command &command);

  // This is how we help the client out when they want to work with detector
  // ids.
  void fillDetectorMap(std::map<DetectorIdType, size_t> &toFill) const;

  size_t nDetectors() const;
  size_t nPathComponents() const;

  Node const *const nodeAt(size_t index) const;
  V3D sourcePos() const; // TODO REMOVE!
  V3D samplePos() const; // TODO REMOVE!

  size_t samplePathIndex() const;
  size_t sourcePathIndex() const;

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

private:
  void init();

  /// Pointers to all known detectors in the instrument tree
  CowPtr<std::vector<Detector const *>> m_detectorVec;
  /// Pointers to all known path components in the instrument tree
  CowPtr<std::vector<PathComponent const *>> m_pathVec;
  CowPtr<std::vector<Node>> m_nodes;
  /// PathComponent vector index of the source
  size_t m_sourceIndex;
  /// PathComponent vector index of the sample
  size_t m_sampleIndex;
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;
using InstrumentTree_uptr = std::unique_ptr<const InstrumentTree>;

#endif
