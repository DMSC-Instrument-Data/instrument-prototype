#include "gtest/gtest.h"
#include "RotateCommand.h"
#include "MockTypes.h"
#include <Eigen/Geometry>

TEST(rotate_command_test, test_is_meta_data){

    RotateCommand rotate(Eigen::Quaterniond::Identity());
    EXPECT_FALSE(rotate.isMetaDataCommand()) << "Rotations should be recursive";
}

/*
TEST(rotate_command_test, test_execute_when_MockComponent_unique) {

  // Our starting component.
  auto mockComponent = new MockComponent;
  // The delta pos method should be called on the orignal/unique component
  EXPECT_CALL(*mockComponent, rotateBy(_)).Times(1);
  // No copying of the component should be invoked.
  EXPECT_CALL(*mockComponent, clone()).Times(0);
  // Cow make around a single reference count to the component
  CowPtr<Component> component(mockComponent);

  RotateCommand command(Eigen::Quaterniond::Identity());
  EXPECT_FALSE(command.execute(component))
      << "MoveCommand should indicate NO copy made";

  EXPECT_TRUE(Mock::VerifyAndClear(mockComponent));
}
*/
