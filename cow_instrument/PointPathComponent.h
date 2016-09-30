#ifndef POINT_PATH_COMPONENT_H
#define POINT_PATH_COMPONENT_H

#include "PathComponent.h"
#include <Eigen/Core>
#include "Component.h"
#include "IdType.h"

/**
 * CRTP type to do boiler plate work of creating PathComponents that behave like
 * points from a neutronic perspective.
 */
template <typename T> class PointPathComponent : public PathComponent {

public:
  PointPathComponent(Eigen::Vector3d pos, ComponentIdType id);
  virtual Eigen::Vector3d getPos() const override;
  virtual Eigen::Quaterniond getRotation() const override;
  virtual void shiftPositionBy(const Eigen::Vector3d &pos) override;
  virtual void rotate(const Eigen::Vector3d &axis, const double &theta,
                      const Eigen::Vector3d &center) override;
  virtual void rotate(const Eigen::Affine3d &transform,
                      const Eigen::Quaterniond &rotationPart) override;
  virtual bool equals(const Component &other) const override;
  virtual void registerContents(
      std::vector<const Detector *> &detectorLookup,
      std::vector<const PathComponent *> &pathLookup) const override;
  virtual ComponentIdType componentId() const override;
  virtual std::string name() const override;

  double length() const override;
  Eigen::Vector3d entryPoint() const override;
  Eigen::Vector3d exitPoint() const override;
  virtual ~PointPathComponent() {}
  virtual PointPathComponent<T> *clone() const override;

  bool operator==(const PointPathComponent<T> &other) const;
  bool operator!=(const PointPathComponent<T> &other) const;

private:
  Eigen::Vector3d m_pos;
  Eigen::Quaterniond m_rotation;
  ComponentIdType m_componentId;
};

template <typename T>
PointPathComponent<T>::PointPathComponent(Eigen::Vector3d pos, ComponentIdType id)
    : m_pos(pos), m_rotation(Eigen::Quaterniond::Identity()), m_componentId(id) {}

template <typename T> Eigen::Vector3d PointPathComponent<T>::getPos() const {
  return m_pos;
}

template <typename T>
Eigen::Quaterniond PointPathComponent<T>::getRotation() const {
  return m_rotation;
}

template <typename T>
void PointPathComponent<T>::shiftPositionBy(const Eigen::Vector3d &pos) {
  m_pos += pos;
}

template <typename T>
void PointPathComponent<T>::rotate(const Eigen::Vector3d& axis, const double& theta, const Eigen::Vector3d& center) {
    using namespace Eigen;
    Affine3d A = Translation3d(center) * AngleAxisd(theta, axis) * Translation3d(-center);
    m_pos = A * m_pos;
    // Update the absolute rotation of this detector around own center.
    m_rotation = A.rotation() * m_rotation;
}

template <typename T>
void PointPathComponent<T>::rotate(const Eigen::Affine3d &transform,
                                   const Eigen::Quaterniond &rotationPart) {
  m_pos = transform * m_pos;
  // Update the absolute rotation of this detector around own center.
  m_rotation = rotationPart * m_rotation;
}

template <typename T>
PointPathComponent<T> *PointPathComponent<T>::clone() const {
  return new T(*static_cast<const T *>(this));
}

template <typename T> std::string PointPathComponent<T>::name() const {
  return static_cast<const T *>(this)->getname();
}

template <typename T>
bool PointPathComponent<T>::equals(const Component &other) const {
  bool equals = false;
  if (auto *otherPointPathComponent =
          dynamic_cast<const PointPathComponent<T> *>(&other)) {
    equals = (*this == *otherPointPathComponent);
  }
  return equals;
}

template <typename T>
bool PointPathComponent<T>::
operator==(const PointPathComponent<T> &other) const {
  return m_componentId == other.m_componentId && m_pos == other.m_pos;
}

template <typename T>
bool PointPathComponent<T>::
operator!=(const PointPathComponent<T> &other) const {
  return !(*this == other);
}

template <typename T>
void PointPathComponent<T>::registerContents(
    std::vector<const Detector *> &,
    std::vector<const PathComponent *> &pathLookup) const {
  // This is not a detector
  pathLookup.push_back(this);
}

template <typename T>
ComponentIdType PointPathComponent<T>::componentId() const {
  return m_componentId;
}

template <typename T> double PointPathComponent<T>::length() const {
  return 0; // This is a point component.
}

template <typename T> Eigen::Vector3d PointPathComponent<T>::entryPoint() const {
  return getPos(); // This is a point component
}

template <typename T> Eigen::Vector3d PointPathComponent<T>::exitPoint() const {
  return getPos(); // This is a point component
}


#endif
