#include "Node.h"
#include "Command.h"
#include "Component.h"

Node::Node(size_t previous, CowPtr<Component> contents,
           std::string name, unsigned int version)
    : m_previous(previous), m_contents(contents), m_name(name),
      m_version(version) {}

Node::Node(CowPtr<Component> contents, std::string name, unsigned int version)
    : m_previous(-1), m_contents(contents), m_name(name),
      m_version(version) {}

Node::~Node() {}

void Node::doModify(const Command &command) {
  // Running the command on the cow component, will automatically cause a deep
  // copy of that component
  command.execute(*m_contents);
}

void Node::addChild(size_t child) { m_next.push_back(child); }

bool Node::hasParent() const { return m_previous >= 0; }

bool Node::hasChildren() const { return m_next.size() > 0; }

size_t Node::parent() const { return m_previous; }

size_t Node::child(size_t index) const {
  if (index >= m_next.size()) {
    throw std::invalid_argument("Index is out of range");
  }
  return m_next[index];
}

unsigned int Node::version() const { return m_version; }

std::string Node::name() const { return m_name; }

const Component &Node::const_ref() const { return m_contents.const_ref(); }
