#include "MoveCommand.h"
#include "Component.h"

MoveCommand::MoveCommand(V3D offset) : m_offset(offset) {}

bool MoveCommand::execute(CowPtr<Component> &component) const {
  component->deltaPos(m_offset); // De-referenceing forces a copy
  return true; // If you're moving, components are being written to. Copy
               // happened on previous line.
}

bool MoveCommand::isMetaDataCommand() const { /*Moving is recursive*/
  return false;
}
