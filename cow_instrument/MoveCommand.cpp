#include "MoveCommand.h"
#include "Component.h"

MoveCommand::MoveCommand(V3D offset) : m_offset(offset) {}

void MoveCommand::execute(Component &component) const {

  auto currentPos = component.getPos();
  component.setPos(V3D{m_offset[0] + currentPos[0], m_offset[1] + currentPos[1],
                       m_offset[2] + currentPos[2]});
}
