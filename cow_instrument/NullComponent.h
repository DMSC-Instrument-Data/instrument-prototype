#ifndef NULLCOMPONENT_H
#define NULLCOMPONENT_H

#include "Component.h"

class NullComponent : public Component {
  NullComponent() = default;
  V3D getPos() const override;
  void setPos(const V3D &pos) override;
  virtual ~NullComponent();
  NullComponent *clone() const override;
  bool equals(const Component &other) const override;
};

#endif
