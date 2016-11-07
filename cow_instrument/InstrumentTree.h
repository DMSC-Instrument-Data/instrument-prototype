#ifndef INSTRUMENT_TREE_H
#define INSTRUMENT_TREE_H

#include <memory>
#include <vector>
#include <map>
#include <Eigen/Core>
#include "cow_ptr.h"
#include "IdType.h"
#include "ComponentInfo.h"

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
  InstrumentTree(std::shared_ptr<Component> componentRoot);

  InstrumentTree(std::vector<Node> &&nodes);

  InstrumentTree(CowPtr<std::vector<Node>> nodes);

  const Node &root() const;
  const ComponentProxy &rootProxy() const;

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
  const ComponentProxy &proxyAt(size_t index) const;

  size_t samplePathIndex() const;
  size_t sourcePathIndex() const;

  std::vector<ComponentProxy>::const_iterator begin() const {
    return m_componentInfo.begin();
  }
  std::vector<ComponentProxy>::const_iterator end() const {
    return m_componentInfo.end();
  }
  std::vector<ComponentProxy>::const_iterator cbegin() const {
    return m_componentInfo.cbegin();
  }
  std::vector<ComponentProxy>::const_iterator cend() const {
    return m_componentInfo.cend();
  }

  // TODO. Had to add this for serialization. We could do better.
  CowPtr<std::vector<Node>>::RefPtr unsafeContents() const;

  size_t nodeSize() const;
  size_t componentSize() const;
  /// Enable use to determine all sub-components.
  std::vector<size_t> subTreeIndexes(size_t proxyIndex) const;

private:
  void init();
  CowPtr<std::vector<Node>> m_nodes;
  /// PathComponent vector index of the source
  size_t m_sourceIndex;
  /// PathComponent vector index of the sample
  size_t m_sampleIndex;
  /// vector of proxies and relevant pointers
  ComponentInfo m_componentInfo;
  /// Component root
  std::shared_ptr<Component>
      m_componentRoot; // TODO. This needs to change to a unique_ptr
};

using InstrumentTree_const_uptr = std::unique_ptr<const InstrumentTree>;
using InstrumentTree_uptr = std::unique_ptr<const InstrumentTree>;

#endif
