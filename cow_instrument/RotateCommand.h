#ifndef ROTATECOMMAND_H
#define ROTATECOMMAND_H

#include "Command.h"
#include <Eigen/Geometry>

class RotateCommand : public Command {

public:
    RotateCommand(Eigen::Quaterniond rotation);
    virtual bool execute(CowPtr<Component> &component) const override;
    virtual bool isMetaDataCommand() const override;
private:
    Eigen::Quaterniond m_rotation;
};

#endif
