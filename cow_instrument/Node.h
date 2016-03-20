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
  Node(Node const * const previous, std::unique_ptr<Node>&& next,
       CowPtr<Component> contents);

  Node(Node const * const  previous, CowPtr<Component> contents);

  Node(CowPtr<Component> contents);

  ~Node();

  // What this will return is the new root node (instrument) with the command
  // applied.
  std::unique_ptr<InstrumentTree> modify(const Command &command) const;

  bool hasParent() const;

  bool hasChildren() const;

  void addChild(std::unique_ptr<Node>&& child);

  //std::vector<std::unique_ptr<const Node>> children() const;
  Node const * const parent() const;

  // Provide read-only access outside of modify.
  const Component &const_ref() const;

  const Node& child(size_t index) const;

  size_t nChildren() const {return m_next.size();}

private:
  Node const *const parentPtr() const;
  Node const *const obtainRoot() const;

  void doModify(const Command &command);

  std::unique_ptr<Node> smartCopy(const Command &command,
                                  const Component &component,
                                  Node const * const newPrevious,
                                  bool cascade) const;
  Node const * const m_previous;          // parent
  std::vector<std::unique_ptr<const Node>> m_next; // Children
  CowPtr<Component> m_contents;
};

using Node_sptr = std::shared_ptr<Node>;
using Node_const_sptr = std::shared_ptr<const Node>;
using Node_const_wptr = std::weak_ptr<const Node>;

using Node_uptr = std::unique_ptr<Node>;
using Node_const_uptr = std::unique_ptr<const Node>;
#endif
