#ifndef COMPOSITE_COMPONENT_H
#define COMPOSITE_COMPONENT_H

#include "Component.h"
#include <vector>
#include <memory>

class Detector;

class CompositeComponent : public Component {
public:
  CompositeComponent() = default;
  ~CompositeComponent() = default;
  V3D getPos() const override;
  void deltaPos(const V3D &pos) override;
  CompositeComponent *clone() const override;
  bool equals(const Component &other) const override;
  void addComponent(std::shared_ptr<Component> child);
  size_t size() const {return m_children.size();}
  std::shared_ptr<const Component> getChild(size_t index) const;
  void registerDetectors(std::map<size_t, const Detector*>& lookup) const override;

private:
  std::vector<std::shared_ptr<Component>> m_children;
};

using CompositeComponent_sptr = std::shared_ptr<CompositeComponent>;

#endif
