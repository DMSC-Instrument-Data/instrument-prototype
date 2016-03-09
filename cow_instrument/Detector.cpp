#include "Detector.h"

Detector::Detector(size_t id, const V3D &pos)
    : m_id(id), m_pos(pos) {}

V3D Detector::getPos() const { return m_pos; }

void Detector::setPos(const V3D &pos) { m_pos = pos; }


Detector *Detector::clone() const {


  return new Detector(this->m_id, this->m_pos);
}

bool Detector::equals(const Component &other) const{
  if (auto *otherDetector = dynamic_cast<const Detector *>(&other)) {
    return otherDetector->id() == this->id();
  }
  return false;
}

Detector::~Detector() {}
