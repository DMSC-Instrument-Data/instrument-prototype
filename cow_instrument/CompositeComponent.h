#ifndef COMPOSITE_COMPONENT_H
#define COMPOSITE_COMPONENT_H

#include "Component.h"
#include <vector>
#include <memory>

class Detector;

class CompositeComponent : public Component {
public:
  CompositeComponent(ComponentIdType componentId);
  ~CompositeComponent() = default;
  V3D getPos() const override;
  void deltaPos(const V3D &pos) override;
  CompositeComponent *clone() const override;
  bool equals(const Component &other) const override;
  void addComponent(std::unique_ptr<Component>&& child);
  size_t size() const {return m_children.size();}
  const Component& getChild(size_t index) const;
  void registerContents(std::map<size_t, const Detector*>& lookup) const override;
  ComponentIdType componentId() const override;
private:
  const ComponentIdType m_componentId;
  std::vector<std::unique_ptr<Component>> m_children;


};

using CompositeComponent_uptr = std::unique_ptr<const CompositeComponent>;

#endif
