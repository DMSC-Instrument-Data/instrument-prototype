#include "Node.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

// The fixture for testing class Foo.
class NodeTest : public ::testing::Test {
protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  NodeTest() {
    // You can do set-up work for each test here.
  }

  virtual ~NodeTest() {
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

TEST_F(NodeTest, test_construction_isolated) {

  CowPtr<Component> contents(new MockComponent);
  Node node(contents);

  EXPECT_FALSE(node.hasParent());
  EXPECT_FALSE(node.hasChild());
}

TEST_F(NodeTest, test_indirection) {

  MockComponent mockComponent;
  EXPECT_CALL(mockComponent, getPos()).WillRepeatedly(Return(V3D{1, 1, 1}));

  CowPtr<Component> contents(new MockComponent);
  Node node(contents);

  node.const_ref().getPos();
  EXPECT_TRUE(Mock::VerifyAndClear(&mockComponent));
}

TEST_F(NodeTest, test_construction_with_parent) {

  Node_sptr parent =
      std::make_shared<Node>(CowPtr<Component>(new MockComponent));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent, contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_FALSE(node.hasChild());
}

TEST_F(NodeTest, test_construction_with_parent_and_child) {

  Node_sptr parent =
      std::make_shared<Node>(CowPtr<Component>(new MockComponent));
  Node_sptr child =
      std::make_shared<Node>(CowPtr<Component>(new MockComponent));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent, child, contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_TRUE(node.hasChild());
}

TEST_F(NodeTest, test_modify) {

  /*
    we start like this
    a -> b -> c
  */

  MockComponent *a_contents = new MockComponent;
  EXPECT_CALL(*a_contents, equals(_)).WillOnce(Return(false));
  MockComponent *b_contents = new MockComponent;
  EXPECT_CALL(*b_contents, equals(_)).WillOnce(Return(false));
  MockComponent *c_contents = new MockComponent;
  EXPECT_CALL(*c_contents, equals(_)).WillOnce(Return(true));

  auto a = std::make_shared<Node>(CowPtr<Component>(a_contents));
  auto b = std::make_shared<Node>(a, CowPtr<Component>(b_contents));
  a->addChild(b);
  auto c = std::make_shared<Node>(b, CowPtr<Component>(c_contents));
  b->addChild(c);

  /*
    we then modify c.
    so we should now have a shallow copied node tree,

    a`-> b` -> |c`|
  */

  // We would therefore expect the command to be executed on c_contents
  MockCommmand command;
  EXPECT_CALL(command, execute(_)).Times(1);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone()).WillOnce(Return(new MockComponent));

  auto newTree = c->modify(command);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));

  // Sanity check newTree. We expect all nodes to be different.
  EXPECT_NE(newTree.get(), a.get());                   // compare a to a`
  EXPECT_NE(newTree->child().get(), a->child().get()); // compare b to b`
  EXPECT_NE(newTree->child()->child().get(),
            a->child()->child().get()); // compare c to c`

  // However, only the contents of one component should be different.
  EXPECT_EQ(&newTree->const_ref(), &a->const_ref());
  EXPECT_EQ(&newTree->child()->const_ref(), &b->const_ref());
  EXPECT_NE(&newTree->child()->child()->const_ref(), &c->const_ref());
}
}
