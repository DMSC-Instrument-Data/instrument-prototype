#include "MoveCommand.h"
#include "Component.h"

MoveCommand::MoveCommand(V3D offset) : m_offset(offset) {}

void MoveCommand::execute(Component &component) const {
  component.deltaPos(m_offset);
}

bool MoveCommand::isMetaDataCommand() const { return true; }
