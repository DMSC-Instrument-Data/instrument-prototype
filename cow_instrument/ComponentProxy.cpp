#include "ComponentProxy.h"

void ComponentProxy::addChild(size_t child) { m_next.emplace_back(child); }

ComponentProxy::ComponentProxy(Component const *const contents)
    : m_previous(-1), m_contents(contents) {}

ComponentProxy::ComponentProxy(size_t previous, Component const *const contents)
    : m_previous(previous), m_contents(contents) {}

ComponentProxy::ComponentProxy(size_t previous, Component const *const contents,
                               std::vector<size_t> &&children)
    : m_previous(previous), m_contents(contents), m_next(std::move(children)) {}

bool ComponentProxy::hasParent() const { return m_previous >= 0; }

bool ComponentProxy::hasChildren() const { return m_next.size() > 0; }

size_t ComponentProxy::parent() const { return m_previous; }

size_t ComponentProxy::child(size_t index) const {
  if (index >= m_next.size()) {
    throw std::invalid_argument("Index is out of range");
  }
  return m_next[index];
}

const Component &ComponentProxy::const_ref() const { return *m_contents; }

size_t ComponentProxy::nChildren() const { return m_next.size(); }

const std::vector<size_t> &ComponentProxy::children() const { return m_next; }
