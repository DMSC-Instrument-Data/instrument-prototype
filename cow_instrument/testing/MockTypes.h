#ifndef MOCKTYPES_H
#define MOCKTYPES_H

#include "Component.h"
#include "Command.h"
#include "gmock/gmock.h"

class MockComponent : public Component {
public:
  MOCK_CONST_METHOD0(getPos, V3D());
  MOCK_METHOD1(deltaPos, void(const V3D &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  ~MockComponent() {}
};

class MockCommmand : public Command {
public:
  MOCK_CONST_METHOD1(execute, void(Component &));
  ~MockCommmand() {}
};

#endif
