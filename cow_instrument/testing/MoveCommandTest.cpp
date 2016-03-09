#include "MoveCommand.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

// The fixture for testing class Foo.
class MoveCommandTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  MoveCommandTest() {
    // You can do set-up work for each test here.
  }

  virtual ~MoveCommandTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

};

TEST_F(MoveCommandTest, test_execute) {

    MockComponent component;
    EXPECT_CALL(component, setPos(_)).Times(1);

    MoveCommand command(V3D{1,2,3});
    command.execute(component);

    EXPECT_TRUE(Mock::VerifyAndClear(&component));

}


}
