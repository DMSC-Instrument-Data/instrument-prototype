#ifndef NULLCOMPONENT_H
#define NULLCOMPONENT_H

#include "Component.h"

class NullComponent : public Component {
public:
  NullComponent() = default;
  virtual Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  void shiftPositionBy(const Eigen::Vector3d &delta) override;
  void rotate(const Eigen::Vector3d& axis, const double& theta, const Eigen::Vector3d& center) override;
  void rotate(const Eigen::Affine3d &transform,
              const Eigen::Quaterniond &rotationPart) override;
  virtual ~NullComponent();
  NullComponent *clone() const override;
  bool equals(const Component &other) const override;
  void registerContents(std::vector<const Detector *> &,
                        std::vector<const PathComponent *> &) const override;
  ComponentIdType componentId() const override;
  std::string name() const override;
  static const std::string TypeName;
  virtual void accept(ComponentVisitor *visitor) const override{};
};

#endif
