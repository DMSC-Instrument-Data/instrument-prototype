#ifndef DETECTORCOMPONENT_H
#define DETECTORCOMPONENT_H

#include "Detector.h"
#include "Component.h"
#include <Eigen/Geometry>

class DetectorComponent : public Detector {

public:
  DetectorComponent(ComponentIdType componentId, DetectorIdType detectorId,
                    const Eigen::Vector3d &pos);

  DetectorComponent(const DetectorComponent &) = default;
  DetectorComponent &operator=(const DetectorComponent &) = default;

  Eigen::Vector3d getPos() const override;
  Eigen::Quaterniond getRotation() const;
  virtual ~DetectorComponent();
  DetectorComponent *clone() const override;
  bool equals(const Component &other) const override;
  DetectorIdType detectorId() const override;
  ComponentIdType componentId() const override;
  virtual void registerContents(SOASource &info) const override;
  virtual void registerContents(SOASource &info,
                                size_t parentIndex) const override;
  std::string name() const override;
  virtual bool accept(ComponentVisitor *visitor) const override;

private:
  const DetectorIdType m_detectorId;
  const ComponentIdType m_componentId;
  Eigen::Vector3d m_pos;
  Eigen::Quaterniond m_rotation;
  size_t m_detectorIndex;
};

#endif
