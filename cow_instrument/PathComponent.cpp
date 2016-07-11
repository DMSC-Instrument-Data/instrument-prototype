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

PathComponent::~PathComponent() {}
