#include "MoveCommand.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(move_component_test, test_execute_when_MockComponent_unique) {

  // Our starting component.
  auto mockComponent = new MockComponent;
  // The delta pos method should be called on the orignal/unique component
  EXPECT_CALL(*mockComponent, shiftPositionBy(_)).Times(1);
  // No copying of the component should be invoked.
  EXPECT_CALL(*mockComponent, clone()).Times(0);
  // Cow make around a single reference count to the component
  CowPtr<Component> component(mockComponent);

  MoveCommand command(V3D{1, 2, 3});
  EXPECT_FALSE(command.execute(component))
      << "MoveCommand should indicate NO copy made";

  EXPECT_TRUE(Mock::VerifyAndClear(mockComponent));
}

TEST(move_component_test, test_execute_when_MockComponent_copyable) {

  // Our starting component.
  auto mockComponent = new MockComponent;
  // The object that we get when the starting component is cloned.
  auto clonedMockComponent = new MockComponent;
  EXPECT_CALL(*clonedMockComponent, shiftPositionBy(_)).Times(1);
  EXPECT_CALL(*mockComponent, clone()).WillOnce(Return(clonedMockComponent));

  CowPtr<Component> component(mockComponent);
  // We create a second reference to the component cow, so that a copy now must
  // take place
  auto originalReference = component;

  MoveCommand command(V3D{1, 2, 3});
  EXPECT_TRUE(command.execute(component))
      << "MoveCommand should indicate copy made";

  EXPECT_TRUE(originalReference.unique())
      << "Reference count to original should drop to 1";
  EXPECT_TRUE(Mock::VerifyAndClear(mockComponent));
  EXPECT_TRUE(Mock::VerifyAndClear(clonedMockComponent));
}
}
