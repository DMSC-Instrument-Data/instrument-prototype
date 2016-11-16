#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <string>
#include "IdType.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "ComponentInfo.h"

class Detector;
class PathComponent;

class Component {
public:
  virtual ~Component() {}
  virtual Component *clone() const = 0;
  virtual bool equals(const Component &other) const = 0;
  virtual void registerContents(ComponentInfo &info) const = 0;
  virtual void registerContents(ComponentInfo &info,
                                size_t parentIndex) const = 0;
  virtual ComponentIdType componentId() const = 0;
  virtual std::string name() const = 0;
  virtual bool accept(class ComponentVisitor *visitor) const = 0;

  virtual Eigen::Vector3d getPos() const = 0; // TODO. Remove to avoid mistakes
  virtual Eigen::Quaterniond
  getRotation() const = 0; // TODO. Remove to avoid mistakes
};

#endif
