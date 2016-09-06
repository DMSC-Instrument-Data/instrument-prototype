#include "ParabolicGuide.h"
#include "ComponentVisitor.h"
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
                               Eigen::Vector3d position)
    : m_componentId(componentId), m_a(a), m_h(h), m_position(position),
      m_rotation(Eigen::Quaterniond::Identity()) {
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

Eigen::Vector3d ParabolicGuide::getPos() const { return m_position; }

Eigen::Quaterniond ParabolicGuide::getRotation() const { return m_rotation; }

void ParabolicGuide::shiftPositionBy(const Eigen::Vector3d &pos) { m_position = pos; }

void ParabolicGuide::rotate(const Eigen::Vector3d &axis, const double &theta, const Eigen::Vector3d &center)
{
  using namespace Eigen;
  const Affine3d transform =
      Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
  m_position = transform * m_position;
  // Update the absolute rotation of this detector around own center.
  m_rotation = transform.rotation() * m_rotation;
}

void ParabolicGuide::rotate(const Eigen::Affine3d &transform,
                            const Eigen::Quaterniond &rotationPart) {
  m_position = transform * m_position;
  // Update the absolute rotation of this detector around own center.
  m_rotation = rotationPart * m_rotation;
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

double ParabolicGuide::a() const { return m_a; }

double ParabolicGuide::h() const { return m_h; }

Eigen::Vector3d ParabolicGuide::entryPoint() const {
  // TODO This is a little hacky. We are assuming beam down x.
  // Would be better to use the reference frame for this.
  auto entry = m_position;
  entry[0] -= m_a;
  return entry;
}

Eigen::Vector3d ParabolicGuide::exitPoint() const {
  // TODO. This is a little hacky. We are assuming beam down x.
  // Would be better to use the reference frame for this.
  auto entry = m_position;
  entry[0] += m_a;
  return entry;
}

bool ParabolicGuide::accept(ComponentVisitor *visitor) const
{
    return visitor->visit(this);
}

bool ParabolicGuide::operator==(const ParabolicGuide &other) const {
  return m_componentId == other.m_componentId && m_a == other.m_a &&
         m_h == other.m_h && m_position == other.m_position;
}


