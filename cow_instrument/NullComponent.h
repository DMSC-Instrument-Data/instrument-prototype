#ifndef NULLCOMPONENT_H
#define NULLCOMPONENT_H

#include "Component.h"

class NullComponent : public Component {
public:
  NullComponent() = default;
  virtual Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  virtual ~NullComponent();
  NullComponent *clone() const override;
  bool equals(const Component &) const override;
  void registerContents(LinkedTreeParser &) const override;
  virtual void registerContents(LinkedTreeParser &, size_t) const override;

  ComponentIdType componentId() const override;
  std::string name() const override;
  static const std::string TypeName;
  virtual bool accept(ComponentVisitor *) const override;
};

#endif
