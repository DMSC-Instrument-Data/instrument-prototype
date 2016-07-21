#include "NullComponent.h"

V3D NullComponent::getPos() const { return V3D{0, 0, 0}; }

void NullComponent::shiftPositionBy(const V3D &) {}

NullComponent::~NullComponent() {}

NullComponent *NullComponent::clone() const { return new NullComponent{}; }

bool NullComponent::equals(const Component& other) const{
    return dynamic_cast<const NullComponent*>(&other) != nullptr;
}

void
NullComponent::registerContents(std::vector<const Detector *> &,
                                std::vector<const PathComponent *> &) const {
  // Do nothing.
}

ComponentIdType NullComponent::componentId() const
{
    throw std::runtime_error("NullComponent does not implement componentId");
}

std::string NullComponent::name() const { return NullComponent::TypeName; }

const std::string NullComponent::TypeName = "NullComponent";
