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
  void shiftPositionBy(const Eigen::Vector3d &pos) override;
  void rotate(const Eigen::Vector3d &axis, const double &theta,
              const Eigen::Vector3d &center) override;
  virtual void rotate(const Eigen::Affine3d &transform,
                      const Eigen::Quaterniond &rotationPart) override;
  virtual ~DetectorComponent();
  DetectorComponent *clone() const override;
  bool equals(const Component &other) const override;
  DetectorIdType detectorId() const override;
  ComponentIdType componentId() const override;
  void registerContents(std::vector<const Detector *> &lookup,
                        std::vector<const PathComponent *> &pathComponents,
                        std::vector<size_t> &detectorIndexes,
                        std::vector<size_t> &) override;
  std::string name() const override;
  virtual bool accept(ComponentVisitor *visitor) const override;

private:
  void setIndex(size_t detectorIndex);
  const DetectorIdType m_detectorId;
  const ComponentIdType m_componentId;
  Eigen::Vector3d m_pos;
  Eigen::Quaterniond m_rotation;
  size_t m_detectorIndex;
};

#endif
