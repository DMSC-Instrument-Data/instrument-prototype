#ifndef DETECTOR_H
#define DETECTOR_H

#include "Component.h"

class Detector : public Component {

public:

  Detector(size_t id, const V3D &pos);
  Detector(const Detector&) = default;
  Detector& operator=(const Detector&) = default;

  V3D getPos() const override;
  void setPos(const V3D &pos) override;
  virtual ~Detector();
  Detector* clone() const override;
  bool equals(const Component& other) const override;
  size_t id() const{return m_id;}

private:
  size_t m_id;
  V3D m_pos;


};

#endif
