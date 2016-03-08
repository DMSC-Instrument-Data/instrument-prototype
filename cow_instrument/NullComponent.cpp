#include "NullComponent.h"

V3D NullComponent::getPos() const
{
    return V3D{0,0,0};
}


void NullComponent::setPos(const V3D &)
{

}


void NullComponent::replace(Component *, Component *)
{

}


NullComponent::~NullComponent()
{

}

NullComponent* NullComponent::clone() const {
    return new NullComponent{};
}

const Component &NullComponent::parent() const
{
    return *this;
}
