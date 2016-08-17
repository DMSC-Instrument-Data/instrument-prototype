#ifndef ROTATECOMMAND_H
#define ROTATECOMMAND_H

#include "Command.h"
#include <Eigen/Core>
#include <boost/optional.hpp>

class RotateCommand : public Command {

public:
    RotateCommand(Eigen::Vector3d axis, double theta, Eigen::Vector3d center);
    RotateCommand(Eigen::Vector3d axis, double theta);
    virtual bool execute(CowPtr<Component> &component) const override;
    virtual bool isMetaDataCommand() const override;
private:
    const Eigen::Vector3d m_axis;
    const double m_theta;
    mutable boost::optional<Eigen::Vector3d> m_center;
};

#endif
