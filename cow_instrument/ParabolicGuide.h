#ifndef PARABOLIC_GUIDE_H
#define PARABOLIC_GUIDE_H

#include "PathComponent.h"
#include "IdType.h"
#include <Eigen/Core>

class ParabolicGuide : public PathComponent {

public:
  ParabolicGuide(ComponentIdType componentId, double a, double h,
                 Eigen::Vector3d position);

  Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  virtual ParabolicGuide *clone() const override;
  virtual bool equals(const Component &other) const override;
  virtual void registerContents(ComponentInfo &info) const;
  virtual void registerContents(ComponentInfo &info,
                                size_t parentIndex) const override;
  ComponentIdType componentId() const;
  std::string name() const;

  virtual double length() const override;
  double a() const;
  double h() const;
  virtual Eigen::Vector3d entryPoint() const override;
  virtual Eigen::Vector3d exitPoint() const override;
  virtual bool accept(class ComponentVisitor *visitor) const override;
  bool operator==(const ParabolicGuide &other) const;

private:
  ComponentIdType m_componentId;
  double m_a;
  double m_h;
  Eigen::Vector3d m_position;
  Eigen::Quaterniond m_rotation;
  double m_length;
  size_t m_pathIndex;
};

#endif
