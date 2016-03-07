#include "Detector.h"

Detector::Detector(const V3D &pos) : m_pos(pos) {}

V3D Detector::getPos() const { return m_pos; }

void Detector::setPos(V3D &pos) {}

Detector::~Detector() {}
