#include "Detector.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <array>

using namespace testing;

namespace {

class MockComponent : public Component {
public:
  MOCK_CONST_METHOD0(getPos, V3D());
  MOCK_METHOD1(setPos, void(const V3D &));
  MOCK_METHOD2(replace, void(Component *, Component *));
  MOCK_CONST_METHOD0(clone, Component*());
  MOCK_CONST_METHOD0(parent, const Component&());
  ~MockComponent() {}
};

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
  Detector det(CowPtr<Component>(new MockComponent), input);
  EXPECT_EQ(det.getPos(), input);
}

TEST_F(DetectorTest, test_clone_invokes_upward_cow){

    MockComponent* parent = new MockComponent;
    EXPECT_CALL(*parent, clone()).Times(1);

    V3D input{1, 2, 3};
    Detector det(CowPtr<Component>(parent), input);
    Detector* clone = det.clone();


    // We expect the clone to be a different object from the original.
    EXPECT_NE(clone, &det);

    // We expect the cloned parent to also be a different object from the original parent
    EXPECT_NE(&clone->parent(), &det.parent());

    EXPECT_TRUE(Mock::VerifyAndClear(parent));
    delete clone;
}

TEST_F(DetectorTest, partial_tree_update){

/*
         A
         |
  ---------------
  |             |
  B (detector)  C (detector)

  Updating C, should result in a clone of A, but also B needs
  it's parent reference updated, but B does not need to be cloned itself.



*/

    MockComponent* A = new MockComponent;
    EXPECT_CALL(*A, clone()).Times(1);
    auto parent = CowPtr<Component>(A);
    V3D input{1, 2, 3};
    Detector B(parent, input);
    Detector C(parent, input);

    Detector* clone = C.clone();

    //TODO INCOMPLETE

}



} // namespace
