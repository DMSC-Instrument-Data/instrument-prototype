#ifndef COMPOSITE_COMPONENT_H
#define COMPOSITE_COMPONENT_H

#include "Component.h"
#include <vector>
#include <memory>
#include <string>

class Detector;

class CompositeComponent : public Component {
public:
  CompositeComponent(ComponentIdType componentId,
                     std::string name = std::string(""));
  ~CompositeComponent() = default;
  CompositeComponent(const CompositeComponent &other);
  CompositeComponent &operator=(const CompositeComponent &other);
  virtual Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  CompositeComponent *clone() const override;
  bool equals(const Component &other) const override;
  void addComponent(std::unique_ptr<Component>&& child);
  size_t size() const {return m_children.size();}
  const Component& getChild(size_t index) const;
  void registerContents(LinkedTreeParser &info) const override;
  void registerContents(LinkedTreeParser &info, size_t parentIndex) const override;
  ComponentIdType componentId() const override;
  std::string name() const override;
  virtual bool accept(class ComponentVisitor *visitor) const override;
  std::vector<std::shared_ptr<Component>> children() const;
private:
  ComponentIdType m_componentId;
  std::vector<std::shared_ptr<Component>> m_children;
  std::string m_name;
};

using CompositeComponent_uptr = std::unique_ptr<const CompositeComponent>;

#endif
