#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <string>
#include "IdType.h"
#include "V3D.h"
#include <Eigen/Geometry>

class Detector;
class PathComponent;

class Component {
public:
  virtual V3D getPos() const = 0;
  virtual void shiftPositionBy(const V3D &pos) = 0;
  //virtual void rotatePositionBy(const Eigen::Quaternion& rot) = 0;
  virtual ~Component() {}
  virtual Component *clone() const = 0;
  virtual bool equals(const Component &other) const = 0;
  virtual void
  registerContents(std::vector<const Detector *> &detectorLookup,
                   std::vector<const PathComponent *> &pathLookup) const = 0;
  virtual ComponentIdType componentId() const = 0;
  virtual std::string name() const = 0;
};

#endif
