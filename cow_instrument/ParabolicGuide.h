#ifndef PARABOLIC_GUIDE_H
#define PARABOLIC_GUIDE_H

#include "PathComponent.h"
#include "IdType.h"
#include "V3D.h"

class ParabolicGuide : public PathComponent {

public:
  ParabolicGuide(ComponentIdType componentId, double a, double h,
                 Eigen::Vector3d position);

  Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  virtual void shiftPositionBy(const V3D &pos) override;
  virtual void rotate(const Eigen::Vector3d &axis, const double &theta,
                      const Eigen::Vector3d &center) override;
  virtual void rotate(const Eigen::Affine3d &transform,
                      const Eigen::Quaterniond &rotationPart) override;
  virtual ParabolicGuide *clone() const override;
  virtual bool equals(const Component &other) const override;
  virtual void registerContents(
      std::vector<const Detector *> &lookupDetectors,
      std::vector<const PathComponent *> &lookupPathComponents) const;
  virtual ComponentIdType componentId() const override;
  virtual std::string name() const override;

  virtual double length() const override;
  virtual V3D entryPoint() const override;
  virtual V3D exitPoint() const override;

  bool operator==(const ParabolicGuide &other) const;

private:
  ComponentIdType m_componentId;
  double m_a;
  double m_h;
  Eigen::Vector3d m_position;
  Eigen::Quaterniond m_rotation;
  double m_length;
};

#endif
