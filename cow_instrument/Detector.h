#ifndef DETECTOR_H
#define DETECTOR_H

#include "Component.h"
#include "cow_ptr.h"

class Detector : public Component {

public:

  Detector(CowPtr<Component> parent, const V3D &pos);
  Detector(const Detector&) = default;
  Detector& operator=(const Detector&) = default;

  V3D getPos() const override;
  void setPos(const V3D &pos) override;
  void replace(Component* original, Component* replacement) override;
  virtual ~Detector();
  Detector* clone() const override;
  const Component& parent() const override;

private:
  V3D m_pos;
  mutable CowPtr<Component> m_parent;


};

#endif
