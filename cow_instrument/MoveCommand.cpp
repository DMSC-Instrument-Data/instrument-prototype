#include "MoveCommand.h"
#include "Component.h"

MoveCommand::MoveCommand(V3D pos) : m_pos(pos){}

void MoveCommand::execute(Component &component) const {

    component.setPos(m_pos);

}
