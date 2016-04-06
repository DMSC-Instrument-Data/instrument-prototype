#ifndef DETECTORCOMPONENT_H
#define DETECTORCOMPONENT_H

#include "Detector.h"
#include "Component.h"

class DetectorComponent : public Detector {

public:
  DetectorComponent(ComponentIdType componentId, DetectorIdType detectorId,
                    const V3D &pos);

  DetectorComponent(const DetectorComponent &) = default;
  DetectorComponent &operator=(const DetectorComponent &) = default;

  V3D getPos() const override;
  void deltaPos(const V3D &pos) override;
  virtual ~DetectorComponent();
  DetectorComponent *clone() const override;
  bool equals(const Component &other) const override;
  DetectorIdType detectorId() const override;
  ComponentIdType componentId() const override;
  void
  registerContents(std::vector<const Detector *> &lookup) const override;
  std::string name() const override;

private:
  const DetectorIdType m_detectorId;
  const ComponentIdType m_componentId;
  V3D m_pos;
};

#endif
