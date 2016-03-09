#include "NullComponent.h"

V3D NullComponent::getPos() const { return V3D{0, 0, 0}; }

void NullComponent::setPos(const V3D &) {}

NullComponent::~NullComponent() {}

NullComponent *NullComponent::clone() const { return new NullComponent{}; }

bool NullComponent::equals(const Component& other) const{
    return dynamic_cast<const NullComponent*>(&other) != nullptr;
}
