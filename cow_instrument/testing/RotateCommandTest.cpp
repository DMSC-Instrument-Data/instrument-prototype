#include "gtest/gtest.h"
#include "RotateCommand.h"
#include "MockTypes.h"
#include <Eigen/Geometry>

using Eigen::Vector3d;
using namespace testing;

TEST(rotate_command_test, test_is_meta_data){
    RotateCommand rotate(Vector3d{1,0,0}, 0);
    EXPECT_FALSE(rotate.isMetaDataCommand()) << "Rotations should be recursive";
}


TEST(rotate_command_test, test_execute_when_MockComponent_unique) {
  // Our starting component.
  auto mockComponent = new MockComponent;
  // The delta pos method should be called on the orignal/unique component
  EXPECT_CALL(*mockComponent, rotate(_,_,_)).Times(1);
  // No copying of the component should be invoked.
  EXPECT_CALL(*mockComponent, clone()).Times(0);
  // Cow make around a single reference count to the component
  CowPtr<Component> component(mockComponent);
  RotateCommand command(Vector3d{1,0,0}, 0, Vector3d{0,0,0});
  EXPECT_FALSE(command.execute(component))
      << "RotateCommand should indicate NO copy made";

  EXPECT_TRUE(Mock::VerifyAndClear(mockComponent));
}

TEST(rotate_component_test, test_execute_when_MockComponent_copyable) {

  // Our starting component.
  auto mockComponent = new MockComponent;
  // The object that we get when the starting component is cloned.
  auto clonedMockComponent = new MockComponent;
  EXPECT_CALL(*clonedMockComponent, rotate(_,_,_)).Times(1);
  EXPECT_CALL(*mockComponent, clone()).WillOnce(Return(clonedMockComponent));

  CowPtr<Component> component(mockComponent);
  // We create a second reference to the component cow, so that a copy now must
  // take place
  auto originalReference = component;

  RotateCommand command(Vector3d{1,0,0}, 0, Vector3d{0,0,0});
  EXPECT_TRUE(command.execute(component))
      << "RotateCommand should indicate copy made";

  EXPECT_TRUE(originalReference.unique())
      << "Reference count to original should drop to 1";
  EXPECT_TRUE(Mock::VerifyAndClear(mockComponent));
  EXPECT_TRUE(Mock::VerifyAndClear(clonedMockComponent));
}

