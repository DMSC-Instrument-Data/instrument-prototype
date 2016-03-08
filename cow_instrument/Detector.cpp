#include "Detector.h"

Detector::Detector(CowPtr<Component> parent, const V3D &pos)
    : m_parent(parent), m_pos(pos) {}

V3D Detector::getPos() const { return m_pos; }

void Detector::setPos(const V3D &pos) {

  m_pos = pos;
}

void Detector::replace(Component*, Component*){
    throw std::invalid_argument("Replace not supported on Detector");
}

Detector* Detector::clone() const {

    auto parentCopy = m_parent; // Shallow copy
    parentCopy.copy(); // Deep copy

    return new Detector(parentCopy, this->m_pos);
}

const Component& Detector::parent() const{
    return m_parent.const_ref();
}


Detector::~Detector() {}
