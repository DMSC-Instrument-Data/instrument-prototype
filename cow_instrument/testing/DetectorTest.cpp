#include "Detector.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>
#include "MockTypes.h"

using namespace testing;

namespace {

// The fixture for testing class Foo.
class DetectorTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  DetectorTest() {
    // You can do set-up work for each test here.
  }

  virtual ~DetectorTest() {
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

  // Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(DetectorTest, test_construction) {

  V3D input{1, 2, 3};
  size_t id = 1;
  Detector det(id, input);
  EXPECT_EQ(det.getPos(), input);
  EXPECT_EQ(det.id(), id);
}

TEST_F(DetectorTest, test_equals){

    V3D input{1, 2, 3};
    Detector a(1, input);
    Detector b(2, input); // Different id.

    EXPECT_FALSE(a.equals(b));

    MockComponent c;
    EXPECT_FALSE(a.equals(c));

    Detector d(1, input);
    EXPECT_TRUE(a.equals(d));
}

TEST_F(DetectorTest, test_clone){
    V3D input{1, 2, 3};
    Detector det(1, input);
    Detector* clone = det.clone();

    EXPECT_TRUE(det.equals(*clone));
    EXPECT_NE(&det, clone); // different objects
    delete clone;

}




} // namespace
