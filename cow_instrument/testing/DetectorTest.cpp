#include "Detector.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(detector_test, test_construction) {

  V3D input{1, 2, 3};
  size_t id = 1;
  Detector det(id, input);
  EXPECT_EQ(det.getPos(), input);
  EXPECT_EQ(det.id(), id);
}

TEST(detector_test, test_equals){

    V3D input{1, 2, 3};
    Detector a(1, input);
    Detector b(2, input); // Different id.

    EXPECT_FALSE(a.equals(b));

    MockComponent c;
    EXPECT_FALSE(a.equals(c));

    Detector d(1, input);
    EXPECT_TRUE(a.equals(d));
}

TEST(detector_test, test_clone){
    V3D input{1, 2, 3};
    Detector det(1, input);
    Detector* clone = det.clone();

    EXPECT_TRUE(det.equals(*clone));
    EXPECT_NE(&det, clone); // different objects
    delete clone;

}




} // namespace
