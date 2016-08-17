#ifndef PARABOLIC_GUIDE_H
#define PARABOLIC_GUIDE_H

#include "PathComponent.h"
#include "IdType.h"
#include "V3D.h"

class ParabolicGuide : public PathComponent {

public:
  ParabolicGuide(ComponentIdType componentId, double a, double h, V3D position);

  V3D getPos() const override;
  void shiftPositionBy(const V3D &pos) override;
  void rotate(const Eigen::Vector3d& axis, const double& theta, const Eigen::Vector3d& center) override;
  ParabolicGuide *clone() const override;
  bool equals(const Component &other) const override;
  void registerContents(
      std::vector<const Detector *> &lookupDetectors,
      std::vector<const PathComponent *> &lookupPathComponents) const;
  ComponentIdType componentId() const override;
  std::string name() const override;

  double length() const override;
  V3D entryPoint() const override;
  V3D exitPoint() const override;

  bool operator==(const ParabolicGuide &other) const;

private:
  ComponentIdType m_componentId;
  double m_a;
  double m_h;
  V3D m_position;
  double m_length;
};

#endif
