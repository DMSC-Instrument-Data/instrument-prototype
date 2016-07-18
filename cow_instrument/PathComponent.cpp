#include "PathComponent.h"

double PathComponent::length() const {
  // Treat as a point. Zero length.
  return 0;
}

V3D PathComponent::entryPoint() const {
  // Treat as a point.
  return this->getPos();
}

V3D PathComponent::exitPoint() const {
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
