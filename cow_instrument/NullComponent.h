#ifndef NULLCOMPONENT_H
#define NULLCOMPONENT_H

#include "Component.h"

class NullComponent : public Component {
public:
  NullComponent() = default;
  virtual Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  void shiftPositionBy(const Eigen::Vector3d &) override;
  void rotate(const Eigen::Vector3d &, const double &,
              const Eigen::Vector3d &) override;
  void rotate(const Eigen::Affine3d &, const Eigen::Quaterniond &) override;
  virtual ~NullComponent();
  NullComponent *clone() const override;
  bool equals(const Component &) const override;
  void registerContents(std::vector<const Detector *> &,
                        std::vector<const PathComponent *> &,
                        std::vector<size_t> &, std::vector<size_t> &) override;
  ComponentIdType componentId() const override;
  std::string name() const override;
  static const std::string TypeName;
  virtual bool accept(ComponentVisitor *) const override;
};

#endif
