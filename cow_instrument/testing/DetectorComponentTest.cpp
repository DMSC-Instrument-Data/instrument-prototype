#include "DetectorComponent.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(detector_component_test, test_construction) {

  V3D input{1, 2, 3};
  size_t id = 1;
  DetectorComponent det(ComponentIdType(1), id, input);
  EXPECT_EQ(det.getPos(), input);
  EXPECT_EQ(det.detectorId(), id);
  EXPECT_EQ(det.componentId(), ComponentIdType(1));
}

TEST(detector_component_test, test_equals){

    V3D input{1, 2, 3};
    DetectorComponent a(ComponentIdType(1), 1, input);
    DetectorComponent b(ComponentIdType(2), 2, input); // Different id.

    EXPECT_FALSE(a.equals(b));

    MockComponent c;
    EXPECT_FALSE(a.equals(c));

    DetectorComponent d(ComponentIdType(3), 1, input);
    EXPECT_TRUE(a.equals(d));
}

TEST(detector_component_test, test_clone){
    V3D input{1, 2, 3};
    DetectorComponent det(ComponentIdType(1), 1, input);
    DetectorComponent* clone = det.clone();

    EXPECT_TRUE(det.equals(*clone));
    EXPECT_NE(&det, clone); // different objects
    delete clone;

}




} // namespace
