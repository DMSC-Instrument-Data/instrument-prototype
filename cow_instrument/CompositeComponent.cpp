#include "CompositeComponent.h"

V3D CompositeComponent::getPos() const {
  return m_pos; // TODO we should walk the tree up to get the actual position.
}

void CompositeComponent::setPos(const V3D &pos) { m_pos = pos; }

CompositeComponent *CompositeComponent::clone() const {
  CompositeComponent *product = new CompositeComponent;
  for (size_t i = 0; i < this->size(); ++i) {
    product->addComponent(std::shared_ptr<Component>(m_children[i]->clone()));
  }
  return product;
}

bool CompositeComponent::equals(const Component &other) const {
  if (auto *otherComposite = dynamic_cast<const CompositeComponent *>(&other)) {
    if (otherComposite->size() != this->size()) {
      return false;
    }
    for (size_t i = 0; i < m_children.size(); ++i) {
      if (!m_children[i]->equals(*otherComposite->m_children[i]))
        return false;
    }
    return true;
  }
  return false;
}

void CompositeComponent::addComponent(std::shared_ptr<Component> child) {
  m_children.push_back(child);
}

std::shared_ptr<const Component> CompositeComponent::getChild(size_t index) const {
  if (index >= m_children.size()) {
    throw std::invalid_argument(
        "index out of range in CompositeComponent::getChild");
  } else {
      return m_children[index];
  }
}
