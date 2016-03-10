#ifndef COMPOSITE_COMPONENT_H
#define COMPOSITE_COMPONENT_H

#include "Component.h"
#include <vector>
#include <memory>

class CompositeComponent : public Component {
public:
  CompositeComponent() = default;
  ~CompositeComponent() = default;
  V3D getPos() const override;
  void setPos(const V3D &pos) override;
  CompositeComponent *clone() const override;
  bool equals(const Component &other) const override;
  void addComponent(std::shared_ptr<Component> child);
  size_t size() const {return m_children.size();}

private:
  std::vector<std::shared_ptr<Component>> m_children;
  V3D m_pos;
};

using CompositeComponent_sptr = std::shared_ptr<CompositeComponent>;

#endif
