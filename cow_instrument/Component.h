#ifndef COMPONENT_H
#define COMPONENT_H

#include <array>

using V3D = std::array<double, 3>;

class Component {
public:
  virtual V3D getPos() const = 0;
  virtual void setPos(const V3D& pos) = 0;
  virtual ~Component(){}
  virtual Component* clone() const = 0;
  virtual bool equals(const Component& other) const = 0;
};

#endif
