#ifndef COMPONENTPROXY_H
#define COMPONENTPROXY_H

#include <vector>
#include <cstddef>
#include <cstdint>
#include "IdType.h"

class Component;

/**
 * Components and InstrumentTrees are Immutable. However the DetectorInfo needs
 *a way to
 *
 * 1. Interpret the Tree as a navigatable array(s)
 * 2. Make modifications to properties loosely attached to the immutable tree
 *components
 *
 * This Proxy provides a way to navigate the InstrumentTree similar to a doubly
 *linked-list representation,
 * but without the need for pointers to objects, and hopefully less access to
 *main memory and better cache locality.
 */
class ComponentProxy {
public:
  ComponentProxy(const ComponentIdType &id);

  ComponentProxy(size_t previous, const ComponentIdType &id);

  ComponentProxy(size_t previous, const ComponentIdType &id,
                 std::vector<size_t> &&children);

  bool hasParent() const;

  bool hasChildren() const;

  void addChild(size_t child);

  size_t parent() const;

  size_t child(size_t index) const;

  const std::vector<size_t> &children() const;

  const ComponentIdType componentId() const; // Not strictly needed.

  size_t nChildren() const;

  bool operator==(const ComponentProxy &other) const;
  bool operator!=(const ComponentProxy &other) const;

private:
  /// Parent component, negative index indicates no parent.
  int64_t m_previous;
  /// Next or child nodes (owned)
  std::vector<size_t> m_next; // Children
  /// Identifier for the component.
  ComponentIdType m_componentId;
};

#endif
