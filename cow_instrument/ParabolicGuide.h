#ifndef PARABOLIC_GUIDE_H
#define PARABOLIC_GUIDE_H

#include "PathComponent.h"
#include "IdType.h"
#include "V3D.h"

class ParabolicGuide : public PathComponent {

public:
  ParabolicGuide(ComponentIdType componentId, double a, double h, V3D position);

  V3D getPos() const;
  void shiftPositionBy(const V3D &pos);
  ParabolicGuide *clone() const;
  bool equals(const Component &other) const;
  void registerContents(
      std::vector<const Detector *> &lookupDetectors,
      std::vector<const PathComponent *> &lookupPathComponents) const;
  ComponentIdType componentId() const;
  std::string name() const;

  double length() const;
  double a() const;
  double h() const;
  V3D entryPoint() const;
  V3D exitPoint() const;

  bool operator==(const ParabolicGuide &other) const;

private:
  ComponentIdType m_componentId;
  double m_a;
  double m_h;
  V3D m_position;
  double m_length;
};

#endif
