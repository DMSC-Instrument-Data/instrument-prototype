#include "Node.h"
#include "InstrumentTree.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(node_test, test_construction_isolated) {

  CowPtr<Component> contents(new MockComponent);
  Node node(contents);

  EXPECT_FALSE(node.hasParent());
  EXPECT_FALSE(node.hasChildren());
  EXPECT_EQ(node.version(), 0);
}

TEST(node_test, test_indirection) {

  MockComponent *mockComponent = new MockComponent;
  EXPECT_CALL(*mockComponent, getPos()).WillOnce(Return(V3D{1, 1, 1}));

  CowPtr<Component> contents(mockComponent);
  Node node(contents);

  node.const_ref().getPos();
  EXPECT_TRUE(Mock::VerifyAndClear(&mockComponent));
}

TEST(node_test, test_construction_with_parent) {

  Node_uptr parent(new Node(CowPtr<Component>(new MockComponent)));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent.get(), contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_FALSE(node.hasChildren());
}

TEST(node_test, test_construction_with_parent_and_child) {

  Node_uptr parent(new Node(CowPtr<Component>(new MockComponent)));
  Node_uptr child(new Node(CowPtr<Component>(new MockComponent)));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent.get(), std::move(child), contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_TRUE(node.hasChildren());
}

TEST(node_test, test_clone) {
  Node_uptr parent(new Node(CowPtr<Component>(new MockComponent)));
  Node_uptr child(new Node(CowPtr<Component>(new MockComponent)));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent.get(), std::move(child), contents);
  auto clone = node.clone();

  // New object.
  EXPECT_NE(clone.get(), &node);

  // Version should not be different.
  EXPECT_EQ(clone->version(), node.version());

  // Version should be consistent across children
  EXPECT_EQ(clone->version(), clone->child(0).version());
}

TEST(node_test, test_modify_linear) {

  /*
    we start like this
    a -> b -> c
  */

  auto *a_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*a_contents, equals(_)).WillOnce(Return(false));
  auto *b_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*b_contents, equals(_)).WillOnce(Return(false));
  auto *c_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*c_contents, equals(_)).WillOnce(Return(true));

  Node_uptr a(new Node(CowPtr<Component>(a_contents)));
  Node_uptr b(new Node(a.get(), CowPtr<Component>(b_contents)));
  Node_uptr c(new Node(b.get(), CowPtr<Component>(c_contents)));
  b->addChild(std::move(c));
  a->addChild(std::move(b));

  /*
    we then modify c.
    so we should now have a shallow copied node tree,

    a`-> b` -> |c`|
  */

  // We would therefore expect the command to be executed on c_contents
  MockCommmand command;
  EXPECT_CALL(command, execute(_)).Times(1);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  // Go from a to be to c, and modify that.
  auto newTree = a->child(0).child(0).modify(command);

  // Check that our tree has an incremented version
  EXPECT_EQ(newTree->version(), a->version() + 1);
  // And check further down in the tree too. The version should be the same
  // across the whole tree.
  EXPECT_EQ(newTree->version(), a->child(0).version() + 1);
  // And bottom of tree too. The version should be the same across the whole
  // tree.
  EXPECT_EQ(newTree->version(), a->child(0).child(0).version() + 1);

  // However, only the contents of one component should be different.
  EXPECT_EQ(&newTree->root().const_ref(), a_contents);
  EXPECT_EQ(&newTree->root().child(0).const_ref(), b_contents);
  EXPECT_NE(&newTree->root().child(0).child(0).const_ref(), c_contents);

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(c_contents));
}

TEST(node_test, test_modify_tree) {

  /*
    we start like this

        A
        |
 ------------------
 |                |
 B                C

  */

  MockComponent *a_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*a_contents, equals(_)).WillOnce(Return(false));
  MockComponent *b_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*b_contents, equals(_)).WillOnce(Return(false));
  MockComponent *c_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*c_contents, equals(_)).WillOnce(Return(true));

  Node_uptr a(new Node(CowPtr<Component>(a_contents)));
  Node_uptr b(new Node(a.get(), CowPtr<Component>(b_contents)));
  Node_uptr c(new Node(a.get(), CowPtr<Component>(c_contents)));
  a->addChild(std::move(b));
  a->addChild(std::move(c));

  /*
    we then modify c.
    so we should now have a shallow copied node tree,

        A`
        |
 ------------------
 |                |
 B`                |C`|
  */

  // We would therefore expect the command to be executed on c_contents
  MockCommmand command;
  EXPECT_CALL(command, execute(_)).Times(1);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  // Get to c via a, and modify c
  auto newTree = a->child(1).modify(command);

  // Check that our tree has an incremented version
  EXPECT_EQ(newTree->version(), a->version() + 1);
  // And check further down in the tree too. The version should be the same
  // across the whole tree.
  EXPECT_EQ(newTree->version(), a->child(0).version() + 1);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));

  // However, only the contents of one component should be different.
  EXPECT_EQ(&newTree->root().const_ref(), a_contents);
  EXPECT_EQ(&newTree->root().child(0).const_ref(), b_contents);
  EXPECT_NE(&newTree->root().child(1).const_ref(), c_contents);
}

TEST(node_test, test_tree_cascade) {

  /*
   We are making writes more expensive, but critically reads
   less expensive.

    we start like this

        A
        |
 ------------------
 |                |
 B                C
                  |
                  D


  */

  auto *a_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*a_contents, equals(_)).WillOnce(Return(false));
  auto *b_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*b_contents, equals(_)).WillOnce(Return(false));
  auto *c_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*c_contents, equals(_))
      .WillOnce(Return(true)); // To force an update of c
  auto *d_contents = new NiceMock<MockComponent>();
  EXPECT_CALL(*d_contents, equals(_))
      .WillOnce(Return(
          false)); // Will force and update even though not explicitly requested

  Node_uptr a(new Node(CowPtr<Component>(a_contents)));
  Node_uptr b(new Node(a.get(), CowPtr<Component>(b_contents)));
  Node_uptr c(new Node(a.get(), CowPtr<Component>(c_contents)));
  Node_uptr d(new Node(c.get(), CowPtr<Component>(d_contents)));
  c->addChild(std::move(d));
  a->addChild(std::move(b));
  a->addChild(std::move(c));

  /*
    we then modify c.
    so we should now have a shallow copied node tree,

        A`
        |
 ------------------
 |                |
 B`                |C`|
                  |
                  |D`|

  */

  /* We would therefore expect the command to be executed on c_contents
   * but also on d_contents, as we have to cascade all changes during the
   * wright*/
  MockCommmand command;
  EXPECT_CALL(command, execute(_)).Times(2);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));
  EXPECT_CALL(*d_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  auto newTree = a->child(1).modify(command);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));

  // These contents should be the same
  EXPECT_EQ(&newTree->root().const_ref(), a_contents);
  EXPECT_EQ(&newTree->root().child(0).const_ref(), b_contents);
  // These should not
  EXPECT_NE(&newTree->root().child(1).const_ref(), c_contents);
  EXPECT_NE(&newTree->root().child(1).child(0).const_ref(), d_contents);
}
}
