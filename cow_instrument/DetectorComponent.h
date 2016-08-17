#ifndef DETECTORCOMPONENT_H
#define DETECTORCOMPONENT_H

#include "Detector.h"
#include "Component.h"
#include <Eigen/Geometry>

class DetectorComponent : public Detector {

public:
  DetectorComponent(ComponentIdType componentId, DetectorIdType detectorId,
                    const V3D &pos);

  DetectorComponent(const DetectorComponent &) = default;
  DetectorComponent &operator=(const DetectorComponent &) = default;

  V3D getPos() const override;
  Eigen::Quaterniond getRotation() const;
  void shiftPositionBy(const V3D &pos) override;
  void rotate(const Eigen::Vector3d& axis, const double& theta, const Eigen::Vector3d& center) override;
  virtual ~DetectorComponent();
  DetectorComponent *clone() const override;
  bool equals(const Component &other) const override;
  DetectorIdType detectorId() const override;
  ComponentIdType componentId() const override;
  void registerContents(
      std::vector<const Detector *> &lookup,
      std::vector<const PathComponent *> &pathComponents) const override;
  std::string name() const override;

private:
  const DetectorIdType m_detectorId;
  const ComponentIdType m_componentId;
  Eigen::Vector3d m_pos;
  Eigen::Quaterniond m_rotation;
};

#endif
