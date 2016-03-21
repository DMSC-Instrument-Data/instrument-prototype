#ifndef NODE_H
#define NODE_H

#include "cow_ptr.h"
#include <memory>
#include <tuple>
#include <vector>

class Component;
class Command;
class InstrumentTree;

/**
 * To give us a way of reusing components, and simply updating the association
 *without having
 * to deepcopy the components.
 *
 * Components do not know about Nodes
 */

class Node {
public:
  Node(Node const *const previous, std::unique_ptr<Node> &&next,
       CowPtr<Component> contents, unsigned int version = 0);

  Node(Node const *const previous, CowPtr<Component> contents,
       unsigned int version = 0);

  Node(CowPtr<Component> contents, unsigned int version = 0);

  ~Node();

  // What this will return is the new root node (instrument) with the command
  // applied.
  std::unique_ptr<InstrumentTree> modify(const Command &command) const;

  bool hasParent() const;

  bool hasChildren() const;

  void addChild(std::unique_ptr<Node> &&child);

  Node const *const parent() const;

  // Provide read-only access outside of modify.
  const Component &const_ref() const;

  const Node &child(size_t index) const;

  size_t nChildren() const { return m_next.size(); }

  unsigned int version() const;

  /// Copy, do not increment version, do not modify
  std::unique_ptr<Node> clone() const;

private:

  std::unique_ptr<Node> clone(Node const * const previous) const;

  void doModify(const Command &command);

  /// Copy, increment version, apply command
  std::unique_ptr<Node> smartCopy(const Command &command,
                                  const Component &component,
                                  Node const *const newPrevious,
                                  bool cascade) const;

  Node const *const obtainRoot() const;

  /// Parent node (nullable)
  Node const *const m_previous;
  /// Next or child nodes (owned)
  std::vector<std::unique_ptr<const Node>> m_next; // Children
  /// COW managed contents. The component.
  CowPtr<Component> m_contents;
  /// version of the node.
  unsigned int m_version;
};

using Node_uptr = std::unique_ptr<Node>;
using Node_const_uptr = std::unique_ptr<const Node>;
#endif
