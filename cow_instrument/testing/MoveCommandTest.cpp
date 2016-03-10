#include "MoveCommand.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(move_component_test, test_execute) {

  MockComponent component;
  EXPECT_CALL(component, setPos(_)).Times(1);

  MoveCommand command(V3D{1, 2, 3});
  command.execute(component);

  EXPECT_TRUE(Mock::VerifyAndClear(&component));
}
}
