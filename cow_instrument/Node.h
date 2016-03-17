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
  Node(std::shared_ptr<Node> previous, std::shared_ptr<Node> next,
       CowPtr<Component> contents);

  Node(std::shared_ptr<Node> previous, CowPtr<Component> contents);

  Node(CowPtr<Component> contents);

  ~Node();

  // What this will return is the new root node (instrument) with the command
  // applied.
  std::unique_ptr<InstrumentTree> modify(const Command &command) const;

  bool hasParent() const;

  bool hasChildren() const;

  void addChild(std::shared_ptr<Node> child);

  std::vector<std::shared_ptr<const Node>> children() const;
  std::shared_ptr<const Node> parent() const;

  // Provide read-only access outside of modify.
  const Component &const_ref() const;

  std::shared_ptr<const Node> child(size_t index) const;

private:
  Node const *const parentPtr() const;
  Node const *const obtainRoot() const;

  void doModify(const Command &command);

  std::shared_ptr<Node> smartCopy(const Command &command,
                                  const Component &component,
                                  std::shared_ptr<Node> &newPrevious,
                                  bool cascade) const;
  std::shared_ptr<Node> m_previous;          // parent
  std::vector<std::shared_ptr<const Node>> m_next; // Children
  CowPtr<Component> m_contents;
};

using Node_sptr = std::shared_ptr<Node>;
using Node_const_sptr = std::shared_ptr<const Node>;
using Node_const_wptr = std::weak_ptr<const Node>;

#endif
