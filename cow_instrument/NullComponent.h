#ifndef NULLCOMPONENT_H
#define NULLCOMPONENT_H

#include "Component.h"

class NullComponent : public Component {
public:
  NullComponent() = default;
  V3D getPos() const override;
  void shiftPositionBy(const V3D &delta) override;
  void rotate(const Eigen::Vector3d& axis, const double& theta, const Eigen::Vector3d& center) override;
  virtual ~NullComponent();
  NullComponent *clone() const override;
  bool equals(const Component &other) const override;
  void registerContents(std::vector<const Detector *> &,
                        std::vector<const PathComponent *> &) const override;
  ComponentIdType componentId() const override;
  std::string name() const override;
  static const std::string TypeName;
};

#endif
