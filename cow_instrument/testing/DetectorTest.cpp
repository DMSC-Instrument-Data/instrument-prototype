#include "Detector.h"
#include "gtest/gtest.h"
#include <array>
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

    V3D input{1,2,3};
    Detector det(input);
    EXPECT_EQ(det.getPos(), input);
}

}

}  // namespace


