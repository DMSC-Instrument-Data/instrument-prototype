#include "Node.h"
#include "Command.h"
#include "Component.h"

Node::Node(Node_sptr previous, Node_sptr next, CowPtr<Component> contents)
    : m_previous(previous), m_next(next), m_contents(contents) {}

Node::Node(Node_sptr previous, CowPtr<Component> contents)
    : m_previous(previous), m_next(nullptr), m_contents(contents) {}

Node::Node(CowPtr<Component> contents)
    : m_previous(nullptr), m_next(nullptr), m_contents(contents) {}

Node::~Node() {}

Node_sptr Node::modify(const Command &command) {

  Node *root = this;
  while (root->hasParent()) {
    root = root->parentPtr();
  }

  Node_sptr emptyPrevious(nullptr);
  Node_sptr newRoot =
      root->smartCopy(command, m_contents.const_ref(), emptyPrevious); // New Instrument
  return newRoot;
}

void Node::doModify(const Command &command) {
  // Running the command on the cow component, will automatically cause a deep
  // copy of that component
  command.execute(*m_contents);
}

void Node::addChild(Node_sptr child) { m_next = child; }

Node_sptr Node::smartCopy(const Command &command, const Component &component,
                          Node_sptr &newPrevious) const {

  Node_sptr copy = std::make_shared<Node>(newPrevious, m_contents);
  if(this->hasChild()){
      copy->addChild(this->m_next->smartCopy(command, component, copy));
  }

  if (m_contents->equals(component)) {
    copy->doModify(command);
  }

  return copy;
}

bool Node::hasParent() const { return m_previous.get() != nullptr; }

bool Node::hasChild() const { return m_next.get() != nullptr; }

Node *Node::parentPtr() { return m_previous.get(); }

Node_sptr  Node::child(){
    return m_next;
}

Node_sptr  Node::parent(){
    return m_previous;
}

CowPtr<Component> Node::contents(){
    return m_contents;
}
