#include "CompositeComponent.h"
#include "Detector.h"

CompositeComponent::CompositeComponent(ComponentIdType componentId) : m_componentId(componentId)
{

}

V3D CompositeComponent::getPos() const {

  /*
   An expensive operation on a composite component, but I don't think this
   is commonly
   */
  V3D pos{0, 0, 0};
  for (size_t i = 0; i < m_children.size(); ++i) {
    auto childPos = m_children[i]->getPos();
    pos[0] += childPos[0];
    pos[1] += childPos[1];
    pos[2] += childPos[2];
  }
  pos[0] /= m_children.size();
  pos[1] /= m_children.size();
  pos[2] /= m_children.size();
  return pos;
}

void CompositeComponent::deltaPos(const V3D &delta) {
  for (size_t i = 0; i < m_children.size(); ++i) {
    m_children[i]->deltaPos(delta);
  }
}

CompositeComponent *CompositeComponent::clone() const {
  CompositeComponent *product = new CompositeComponent{m_componentId};
  for (size_t i = 0; i < this->size(); ++i) {
    product->addComponent(std::unique_ptr<Component>(m_children[i]->clone()));
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

void CompositeComponent::addComponent(std::unique_ptr<Component>&& child) {
  m_children.emplace_back(std::move(child));
}

const Component&
CompositeComponent::getChild(size_t index) const {
  if (index >= m_children.size()) {
    throw std::invalid_argument(
        "index out of range in CompositeComponent::getChild");
  } else {
    return *m_children[index];
  }
}

void CompositeComponent::registerContents(std::vector<const Detector *> &lookup) const
{
    for(auto& child : m_children){
        child->registerContents(lookup);
    }
}

ComponentIdType CompositeComponent::componentId() const
{
    return m_componentId;
}
