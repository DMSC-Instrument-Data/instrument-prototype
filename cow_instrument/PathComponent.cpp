#include "PathComponent.h"

double PathComponent::length() const {
  // Treat as a point. Zero length.
  return 0;
}

Eigen::Vector3d PathComponent::entryPoint() const {
  // Treat as a point.
  return this->getPos();
}

Eigen::Vector3d PathComponent::exitPoint() const {
  // Treat as a point.
  return this->getPos();
}

bool PathComponent::isSource() const {
  // Treat as something that is not a source by default
  return false;
}

bool PathComponent::isSample() const {
  // Treat as something that is not a sample by default
  return false;
}

PathComponent::~PathComponent() {}
