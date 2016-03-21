#ifndef DETECTORCOMPONENT_H
#define DETECTORCOMPONENT_H

#include "Detector.h"
#include "Component.h"

class DetectorComponent : public Detector {

public:

  DetectorComponent(size_t id, const V3D &pos);
  DetectorComponent(const DetectorComponent&) = default;
  DetectorComponent& operator=(const DetectorComponent&) = default;

  V3D getPos() const override;
  void deltaPos(const V3D &pos) override;
  virtual ~DetectorComponent();
  DetectorComponent* clone() const override;
  bool equals(const Component& other) const override;
  size_t id() const override{return m_id;}
  void registerDetectors(std::map<size_t, const Detector*>& lookup) const override;

private:
  size_t m_id;
  V3D m_pos;


};

#endif
