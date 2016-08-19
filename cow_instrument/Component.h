#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <string>
#include "IdType.h"
#include <Eigen/Core>
#include <Eigen/Geometry>

class Detector;
class PathComponent;

class Component {
public:
  virtual Eigen::Vector3d getPos() const = 0;
  virtual Eigen::Quaterniond getRotation() const = 0;
  virtual void shiftPositionBy(const Eigen::Vector3d &pos) = 0;
  virtual void rotate(const Eigen::Vector3d& axis, const double& theta, const Eigen::Vector3d& center) = 0;
  virtual void rotate(const Eigen::Affine3d &transform,
                      const Eigen::Quaterniond &rotationPart) = 0;
  virtual ~Component() {}
  virtual Component *clone() const = 0;
  virtual bool equals(const Component &other) const = 0;
  virtual void
  registerContents(std::vector<const Detector *> &detectorLookup,
                   std::vector<const PathComponent *> &pathLookup) const = 0;
  virtual ComponentIdType componentId() const = 0;
  virtual std::string name() const = 0;
  virtual void accept(class ComponentVisitor *visitor) const = 0;
};

#endif
