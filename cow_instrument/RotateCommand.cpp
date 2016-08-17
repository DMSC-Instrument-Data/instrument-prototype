#include "RotateCommand.h"


RotateCommand::RotateCommand(Eigen::Vector3d axis, double theta, Eigen::Vector3d center) : m_axis(axis), m_theta(theta), m_center(center)
{

}

RotateCommand::RotateCommand(Eigen::Vector3d axis, double theta) : m_axis(axis), m_theta(theta)
{

}

bool RotateCommand::execute(CowPtr<Component> &component) const
{
    const bool pendingCopy = component.copyable();
    /*
     Cache the center position (center of rotation) from the first
     component that you meet if the center has not been explicitly provided.
     */
    if(!m_center.is_initialized()){
        m_center = component.const_ref().getPos();
    }
    component->rotate(m_axis, m_theta, *m_center);
    return pendingCopy;
}


bool RotateCommand::isMetaDataCommand() const
{
    return false; // We should recursively apply rotations
}
