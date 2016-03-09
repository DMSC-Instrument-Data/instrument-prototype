#include "Node.h"
#include "Command.h"
#include "Component.h"

Node::Node(Node_sptr previous, Node_sptr next, CowPtr<Component> contents)
    : m_previous(previous), m_contents(contents) {  m_next.push_back(next); }

Node::Node(Node_sptr previous, CowPtr<Component> contents)
    : m_previous(previous), m_contents(contents) {}

Node::Node(CowPtr<Component> contents)
    : m_previous(nullptr), m_contents(contents) {}

Node::~Node() {}

Node_sptr Node::modify(const Command &command) {

  auto *root = obtainRoot();
  Node_sptr emptyPrevious(nullptr);
  Node_sptr newRoot = root->smartCopy(command, m_contents.const_ref(),
                                      emptyPrevious); // New Instrument
  return newRoot;
}

void Node::doModify(const Command &command) {
  // Running the command on the cow component, will automatically cause a deep
  // copy of that component
  command.execute(*m_contents);
}

void Node::addChild(Node_sptr child) { m_next.push_back(child); }

Node_sptr Node::smartCopy(const Command &command, const Component &component,
                          Node_sptr &newPrevious) const {

  Node_sptr copy = std::make_shared<Node>(newPrevious, m_contents);
  if (this->hasChildren()) {
    for (size_t i = 0; i < this->m_next.size(); ++i) {
      copy->addChild(this->m_next[i]->smartCopy(command, component, copy));
    }
  }

  if (m_contents->equals(component)) {
    copy->doModify(command);
  }

  return copy;
}

bool Node::hasParent() const { return m_previous.get() != nullptr; }

bool Node::hasChildren() const { return m_next.size() > 0; }

Node const *const Node::parentPtr() const { return m_previous.get(); }

std::vector<std::shared_ptr<Node>> Node::children() { return m_next; }

Node_sptr Node::parent() { return m_previous; }

Node const *const Node::obtainRoot() const {
  const Node *root = this;
  while (root->hasParent()) {
    root = root->parentPtr();
  }
  return root;
}

const Component &Node::const_ref() { return m_contents.const_ref(); }
