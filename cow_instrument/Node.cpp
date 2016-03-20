#include "Node.h"
#include "Command.h"
#include "Component.h"
#include "InstrumentTree.h"

Node::Node(Node_sptr previous, Node_sptr next, CowPtr<Component> contents)
    : m_previous(previous), m_contents(contents) {
  m_next.push_back(next);
}

Node::Node(Node_sptr previous, CowPtr<Component> contents)
    : m_previous(previous), m_contents(contents) {}

Node::Node(CowPtr<Component> contents)
    : m_previous(nullptr), m_contents(contents) {}

Node::~Node() {}

std::unique_ptr<InstrumentTree> Node::modify(const Command &command) const {

  auto *root = obtainRoot();
  Node_sptr emptyPrevious(nullptr);
  Node_sptr newRoot = root->smartCopy(
      command, m_contents.const_ref(), emptyPrevious,
      false /*start cascading modifications*/); // New Instrument
  return std::unique_ptr<InstrumentTree>(new InstrumentTree(newRoot));
}

void Node::doModify(const Command &command) {
  // Running the command on the cow component, will automatically cause a deep
  // copy of that component
  command.execute(*m_contents);
}

void Node::addChild(Node_sptr child) { m_next.push_back(child); }

/**
 * Recursively copy Nodes, only cascade the Command over nodes that match
 *criteria and any of their
 * subnodes.
 *
 * @param command
 * @param component
 * @param newPrevious
 * @param cascade
 * @return copy of node.
 */
Node_sptr Node::smartCopy(const Command &command, const Component &component,
                          Node_sptr &newPrevious, bool cascade) const {

  bool doCascade = m_contents->equals(component) || cascade;

  Node_sptr copy = std::make_shared<Node>(newPrevious, m_contents);
  if (this->hasChildren()) {
    for (size_t i = 0; i < this->m_next.size(); ++i) {
      copy->addChild(
          this->m_next[i]->smartCopy(command, component, copy, doCascade));
    }
  }

  if (doCascade) {
    copy->doModify(command);
  }

  return copy;
}

bool Node::hasParent() const { return m_previous.get() != nullptr; }

bool Node::hasChildren() const { return m_next.size() > 0; }

Node const *const Node::parentPtr() const { return m_previous.get(); }

//std::vector<std::shared_ptr<const Node>> Node::children() const { return m_next; }

Node_const_sptr Node::parent() const { return m_previous; }

Node const *const Node::obtainRoot() const {
  const Node *root = this;
  while (root->hasParent()) {
    root = root->parentPtr();
  }
  return root;
}

std::shared_ptr<const Node> Node::child(size_t index) const {
  if (index >= m_next.size()) {
    throw std::invalid_argument("Index is out of range");
  }
  return m_next.at(index);
}

const Component &Node::const_ref() const { return m_contents.const_ref(); }
