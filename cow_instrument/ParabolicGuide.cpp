#include "ParabolicGuide.h"
#include <cmath>

/*

  s is segment length from orgin to vertex

                                                __,..--'""
                                       _,..--'""          |
                  s             _,..-'""                  |
                        _,..-'"                           |
                  _,.-'"                                  |
             _.-'"                                        |
         _.-"                                             | a
      .-'                                                 |
    .'                                                    |
   /                                                      |
  ;                            h                          |
  ;  -----------------------------------------------------|--
   \                                                      |
    `.                                                    |
      `-.                                                 |
         "-._                                             |
             "`-._                                        | a
                  "`-.,_                                  |
                        "`-..,_                           |
                               ""`-..,_                   |
                                       ""`--..,_          |
                                                ""`--..,__


 */

ParabolicGuide::ParabolicGuide(ComponentIdType componentId, double a, double h,
                               V3D position)
    : m_componentId(componentId), m_a(a), m_h(h), m_position(position) {
  if (a < 0) {
    throw std::invalid_argument("Parabola 'a' must be >= 0");
  }
  if (h < 0) {
    throw std::invalid_argument("Parabola 'h' must be >= 0");
  }
  /*
   * Equation of parabolic segment: y = h(1 - x^2/a^2), where h is height to
   *vertex,
   * and a is width to axis of symetry from base.
   *
   * segment length is sqrt(a^2 + 4*h^2) + (a^2/2h)*sinh^-1(2h/a)
   */

  m_length =
      std::sqrt(a * a + 4 * h * h) + (a * a / 2 * h) * std::asinh(2 * h / a);
}

V3D ParabolicGuide::getPos() const { return m_position; }

Eigen::Quaterniond ParabolicGuide::getRotation() const {
  throw std::runtime_error("Not implemented yet");
}

void ParabolicGuide::shiftPositionBy(const V3D &pos) { m_position = pos; }

void ParabolicGuide::rotate(const Eigen::Vector3d &axis, const double &theta, const Eigen::Vector3d &center)
{
    throw std::runtime_error("rotatePositionBy not implemented");
}

ParabolicGuide *ParabolicGuide::clone() const {
  return new ParabolicGuide(m_componentId, m_a, m_h, m_position);
}

bool ParabolicGuide::equals(const Component &other) const {
  bool equals = false;
  if (auto *otherParabolicGuide =
          dynamic_cast<const ParabolicGuide *>(&other)) {
    equals = (*this == *otherParabolicGuide);
  }
  return equals;
}

void ParabolicGuide::registerContents(
    std::vector<const Detector *> &,
    std::vector<const PathComponent *> &pathComponents) const {
  // Register this as a path component. It is not a detector.
  pathComponents.push_back(this);
}

ComponentIdType ParabolicGuide::componentId() const { return m_componentId; }

std::string ParabolicGuide::name() const { return "Parbolic guide"; }

double ParabolicGuide::length() const { return m_length; }

V3D ParabolicGuide::entryPoint() const {
  // TODO This is a little hacky. We are assuming beam down x.
  // Would be better to use the reference frame for this.
  auto entry = m_position;
  entry[0] -= m_a;
  return entry;
}

V3D ParabolicGuide::exitPoint() const {
  // TODO. This is a little hacky. We are assuming beam down x.
  // Would be better to use the reference frame for this.
  auto entry = m_position;
  entry[0] += m_a;
  return entry;
}

bool ParabolicGuide::operator==(const ParabolicGuide &other) const {
  return m_componentId == other.m_componentId && m_a == other.m_a &&
         m_h == other.m_h && m_position == other.m_position;
}


