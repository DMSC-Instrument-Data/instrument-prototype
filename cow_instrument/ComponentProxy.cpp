#include "ComponentProxy.h"
#include "Component.h"
#include <utility>

ComponentProxy::ComponentProxy(const ComponentIdType &id)
    : m_previous(-1), m_componentId(id) {}

ComponentProxy::ComponentProxy(size_t previous, const ComponentIdType &id)
    : m_previous(previous), m_componentId(id) {}

ComponentProxy::ComponentProxy(size_t previous, const ComponentIdType &id,
                               std::vector<size_t> &&children)
    : m_previous(previous), m_componentId(id), m_next(std::move(children)) {}

void ComponentProxy::addChild(size_t child) { m_next.emplace_back(child); }

bool ComponentProxy::hasParent() const { return m_previous >= 0; }

bool ComponentProxy::hasChildren() const { return m_next.size() > 0; }

size_t ComponentProxy::parent() const { return m_previous; }

size_t ComponentProxy::child(size_t index) const {
  if (index >= m_next.size()) {
    throw std::invalid_argument("Index is out of range");
  }
  return m_next[index];
}

size_t ComponentProxy::nChildren() const { return m_next.size(); }

const std::vector<size_t> &ComponentProxy::children() const { return m_next; }

const ComponentIdType ComponentProxy::componentId() const {
  return m_componentId;
}

bool ComponentProxy::operator==(const ComponentProxy &other) const {
  return m_componentId == other.componentId() && m_next == other.children() &&
         m_previous == other.parent();
}

bool ComponentProxy::operator!=(const ComponentProxy &other) const {
  return !(this->operator==(other));
}

