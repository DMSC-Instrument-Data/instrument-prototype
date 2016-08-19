#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "Command.h"
#include "Component.h"

class MoveCommand : public Command {

  // Command interface
public:
  MoveCommand(Eigen::Vector3d pos);
  virtual bool execute(CowPtr<Component> &component) const override;
  virtual bool isMetaDataCommand() const override;
  virtual ~MoveCommand() = default;

private:

  /// Relative offset
  Eigen::Vector3d m_offset;
};

#endif
