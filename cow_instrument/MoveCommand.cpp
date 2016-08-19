#include "MoveCommand.h"
#include "Component.h"

MoveCommand::MoveCommand(Eigen::Vector3d offset) : m_offset(offset) {}

bool MoveCommand::execute(CowPtr<Component> &component) const {
  // TODO. This is not thread safe.
  const bool pendingCopy = component.copyable();
  component->shiftPositionBy(m_offset);
  return pendingCopy;
}

bool MoveCommand::isMetaDataCommand() const { /*Moving is recursive*/
  return false;
}
