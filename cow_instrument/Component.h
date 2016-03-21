#ifndef COMPONENT_H
#define COMPONENT_H

#include <array>
#include <map>

using V3D = std::array<double, 3>;

class Detector;

class Component {
public:
  virtual V3D getPos() const = 0;
  virtual void deltaPos(const V3D& pos) = 0;
  virtual ~Component(){}
  virtual Component* clone() const = 0;
  virtual bool equals(const Component& other) const = 0;
  virtual void registerDetectors(std::map<size_t, const Detector*>& lookup) const = 0;
};

#endif
