#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "Command.h"
#include "Component.h"

class MoveCommand : public Command {

  // Command interface
public:
  MoveCommand(V3D pos);
  void execute(Component &component) const;
  virtual ~MoveCommand() = default;

private:

  /// Relative offset
  V3D m_offset;
};

#endif
