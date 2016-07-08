#include "MoveCommand.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(move_component_test, test_execute) {

  auto mockComponent = new MockComponent;
  EXPECT_CALL(*mockComponent, deltaPos(_)).Times(1);
  CowPtr<Component> component(mockComponent);

  MoveCommand command(V3D{1, 2, 3});
  command.execute(component);

  EXPECT_TRUE(Mock::VerifyAndClear(&component));
}
}
