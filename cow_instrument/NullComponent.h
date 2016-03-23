#ifndef NULLCOMPONENT_H
#define NULLCOMPONENT_H

#include "Component.h"

class NullComponent : public Component {
public:
  NullComponent() = default;
  V3D getPos() const override;
  void deltaPos(const V3D &delta) override;
  virtual ~NullComponent();
  NullComponent *clone() const override;
  bool equals(const Component &other) const override;
  void registerContents(std::map<size_t, const Detector *> &lookup) const override;
  ComponentIdType componentId() const override;
};

#endif
