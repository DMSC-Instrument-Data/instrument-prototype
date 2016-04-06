#include "Node.h"
#include "Command.h"
#include "Component.h"

Node::Node(Node const *const previous, Node_uptr &&next,
           CowPtr<Component> contents, std::string name, unsigned int version)
    : m_previous(previous), m_contents(contents), m_name(name),
      m_version(version) {
  m_next.push_back(std::move(next));
}

Node::Node(Node const *const previous, CowPtr<Component> contents,
           std::string name, unsigned int version)
    : m_previous(previous), m_contents(contents), m_name(name),
      m_version(version) {}

Node::Node(CowPtr<Component> contents, std::string name, unsigned int version)
    : m_previous(nullptr), m_contents(contents), m_name(name),
      m_version(version) {}

Node::~Node() {}

std::unique_ptr<Node> Node::modify(const Command &command) const {

  auto *root = this; // obtainRoot();
  Node_uptr newRoot = root->smartCopy(
      command, m_contents.const_ref(), nullptr,
      false /*start cascading modifications*/); // New Instrument
  return newRoot;
}

void Node::doModify(const Command &command) {
  // Running the command on the cow component, will automatically cause a deep
  // copy of that component
  command.execute(*m_contents);
}

void Node::addChild(Node_uptr &&child) { m_next.push_back(std::move(child)); }

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
Node_uptr Node::smartCopy(const Command &command, const Component &component,
                          Node const *const newPrevious, bool cascade) const {

  bool doCascade = m_contents->equals(component) || cascade;

  // Deep copy the node, but not it's contents. Node version number is
  // incremented on the product.
  Node_uptr copy(new Node(newPrevious, m_contents, m_name, m_version + 1));
  if (this->hasChildren()) {
    for (size_t i = 0; i < this->m_next.size(); ++i) {
      copy->addChild(this->m_next[i]->smartCopy(command, component, copy.get(),
                                                doCascade));
    }
  }

  if (doCascade) {
    copy->doModify(command);
  }

  return copy;
}

bool Node::hasParent() const { return m_previous != nullptr; }

bool Node::hasChildren() const { return m_next.size() > 0; }

Node const *const Node::parent() const { return m_previous; }

Node const *const Node::obtainRoot() const {
  const Node *root = this;
  while (root->hasParent()) {
    root = root->parent();
  }
  return root;
}

const Node &Node::child(size_t index) const {
  if (index >= m_next.size()) {
    throw std::invalid_argument("Index is out of range");
  }
  return *m_next[index];
}

unsigned int Node::version() const { return m_version; }

std::unique_ptr<Node> Node::clone() const { return clone(m_previous); }

std::string Node::name() const { return m_name; }

std::unique_ptr<Node> Node::clone(Node const *const previous) const {
  Node_uptr copy(new Node(previous, m_contents, m_name, m_version));
  if (this->hasChildren()) {
    for (size_t i = 0; i < this->m_next.size(); ++i) {
      copy->addChild(this->m_next[i]->clone(copy.get()));
    }
  }
  return copy;
}

const Component &Node::const_ref() const { return m_contents.const_ref(); }
