#include "Node.h"
#include "Command.h"
#include "Component.h"
#include "NullComponent.h"

Node::Node()
    : m_previous(-1), m_contents(CowPtr<Component>(new NullComponent)),
      m_name(""), m_version(0) {}

Node::Node(size_t previous, CowPtr<Component> contents, std::string name,
           unsigned int version)
    : m_previous(previous), m_contents(contents), m_name(name),
      m_version(version) {}

Node::Node(CowPtr<Component> contents, std::string name, unsigned int version)
    : m_previous(-1), m_contents(contents), m_name(name), m_version(version) {}

Node::Node(size_t previous, CowPtr<Component> contents, std::string name,
           unsigned int version, std::vector<size_t> &&children)
    : m_previous(previous), m_contents(contents), m_name(name),
      m_version(version), m_next(std::move(children)) {}

Node::~Node() {}

bool Node::modify(const Command &command) {
  /*
   * We pass in the cow_ptr<Component> by lvalue ref.
   * It is therefore up to the Command whether it invokes a non-const
   * method call on the cow_ptr. The Command::execute() MUST correctly
   * indicate whether a writeable opeation was performed by returning true.
   */
  return command.execute(m_contents);
}

void Node::addChild(size_t child) { m_next.emplace_back(child); }

bool Node::hasParent() const { return m_previous >= 0; }

bool Node::hasChildren() const { return m_next.size() > 0; }

size_t Node::parent() const { return m_previous; }

size_t Node::child(size_t index) const {
  if (index >= m_next.size()) {
    throw std::invalid_argument("Index is out of range");
  }
  return m_next[index];
}

size_t Node::nChildren() const { return m_next.size(); }

unsigned int Node::version() const { return m_version; }

unsigned int Node::incrementVersion() { return ++m_version; }

std::string Node::name() const { return m_name; }

CowPtr<Component>::RefPtr Node::unsafeContents() const {
  return m_contents.heldValue();
}

const Component &Node::const_ref() const { return m_contents.const_ref(); }

const std::vector<size_t> &Node::children() const { return m_next; }
