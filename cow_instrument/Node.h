#ifndef NODE_H
#define NODE_H

#include "cow_ptr.h"
#include <memory>
#include <tuple>
#include <vector>
#include <string>

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
  Node();

  Node(size_t previous, CowPtr<Component> contents,
       std::string name = "", unsigned int version = 0);

  Node(CowPtr<Component> contents, std::string name = "",
       unsigned int version = 0);

  ~Node();

  bool hasParent() const;

  bool hasChildren() const;

  void addChild(size_t child);

  size_t parent() const;

  // Provide read-only access outside of modify.
  const Component &const_ref() const;

  size_t child(size_t index) const;

  const std::vector<size_t> &children() const;

  size_t nChildren() const;

  unsigned int version() const;

  unsigned int incrementVersion();

  std::string name() const;

  friend class InstrumentTree;

private:
  bool modify(const Command &command);

  /// Parent node, negative index indicates no parent.
  int64_t m_previous;
  /// Next or child nodes (owned)
  std::vector<size_t> m_next; // Children
  /// COW managed contents. The component.
  CowPtr<Component> m_contents;
  /// friendly name.
  std::string m_name;
  /// version of the node.
  unsigned int m_version;
};

using Node_uptr = std::unique_ptr<Node>;
using Node_const_uptr = std::unique_ptr<const Node>;
#endif
