#include "NullComponent.h"
#include "ComponentVisitor.h"

Eigen::Vector3d NullComponent::getPos() const { return Eigen::Vector3d{0, 0, 0}; }

void NullComponent::shiftPositionBy(const Eigen::Vector3d &) {}

void NullComponent::rotate(const Eigen::Vector3d &axis, const double &theta, const Eigen::Vector3d &center)
{
    // Do nothing
}

void NullComponent::rotate(const Eigen::Affine3d &transform,
                           const Eigen::Quaterniond &rotationPart) {
  // Do nothing
}

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

bool NullComponent::accept(ComponentVisitor * visitor) const { return visitor->visit(this); }

const std::string NullComponent::TypeName = "NullComponent";

Eigen::Quaterniond NullComponent::getRotation() const {
  return Eigen::Quaterniond::Identity();
}
