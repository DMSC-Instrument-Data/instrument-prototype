#include "RotateCommand.h"

RotateCommand::RotateCommand(Eigen::Quaterniond rotation) : m_rotation(rotation)
{

}

bool RotateCommand::execute(CowPtr<Component> &component) const
{
    throw std::runtime_error("Not implemented");
}


bool RotateCommand::isMetaDataCommand() const
{
    return false; // We should recursively apply rotations
}
