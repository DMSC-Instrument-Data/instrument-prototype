#include "gtest/gtest.h"
#include "CompositeComponent.h"
#include "MockTypes.h"

using namespace testing;

namespace {

// The fixture for testing class Foo.
class CompositeComponentTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  CompositeComponentTest() {
    // You can do set-up work for each test here.
  }

  virtual ~CompositeComponentTest() {
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


TEST_F(CompositeComponentTest, test_construction){
    CompositeComponent composite;
    EXPECT_EQ(0, composite.size());
}

TEST_F(CompositeComponentTest, test_clone){
    CompositeComponent composite;

    MockComponent* childA = new MockComponent;
    EXPECT_CALL(*childA, clone()).WillRepeatedly(Return(new MockComponent));
    MockComponent* childB = new MockComponent;
    EXPECT_CALL(*childB, clone()).WillRepeatedly(Return(new MockComponent));

    composite.addComponent(std::shared_ptr<MockComponent>(childA));
    composite.addComponent(std::shared_ptr<MockComponent>(childB));
    auto* clone = composite.clone();
    EXPECT_EQ(clone->size(), composite.size());
    delete clone;
    EXPECT_TRUE(Mock::VerifyAndClearExpectations(childA));
    EXPECT_TRUE(Mock::VerifyAndClearExpectations(childB));
}






} // namespace
