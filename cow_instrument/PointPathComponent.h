#ifndef POINT_PATH_COMPONENT_H
#define POINT_PATH_COMPONENT_H

#include "PathComponent.h"
#include "V3D.h"
#include "Component.h"
#include "IdType.h"

/**
 * CRTP type to do boiler plate work of creating PathComponents that behave like
 * points from a neutronic perspective.
 */
template <typename T> class PointPathComponent : public PathComponent {

public:
  PointPathComponent(V3D pos, ComponentIdType id);
  virtual V3D getPos() const override;
  virtual void shiftPositionBy(const V3D &pos) override;

  virtual bool equals(const Component &other) const override;
  virtual void registerContents(
      std::vector<const Detector *> &detectorLookup,
      std::vector<const PathComponent *> &pathLookup) const override;
  virtual ComponentIdType componentId() const override;
  virtual std::string name() const override;
  virtual void accept(ComponentVisitor *visitor) const override{};

  double length() const override;
  V3D entryPoint() const override;
  V3D exitPoint() const override;
  virtual ~PointPathComponent() {}
  virtual PointPathComponent<T> *clone() const override;

  bool operator==(const PointPathComponent<T> &other) const;
  bool operator!=(const PointPathComponent<T> &other) const;

private:
  V3D m_pos;
  ComponentIdType m_componentId;
};

template <typename T>
PointPathComponent<T>::PointPathComponent(V3D pos, ComponentIdType id)
    : m_pos(pos), m_componentId(id) {}

template <typename T> V3D PointPathComponent<T>::getPos() const {
  return m_pos;
}

template <typename T>
void PointPathComponent<T>::shiftPositionBy(const V3D &pos) {
  m_pos[0] += pos[0];
  m_pos[1] += pos[1];
  m_pos[2] += pos[2];
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

template <typename T> V3D PointPathComponent<T>::entryPoint() const {
  return getPos(); // This is a point component
}

template <typename T> V3D PointPathComponent<T>::exitPoint() const {
  return getPos(); // This is a point component
}

#endif
