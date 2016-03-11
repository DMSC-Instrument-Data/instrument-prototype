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
}

TEST(node_test, test_indirection) {

  MockComponent* mockComponent = new MockComponent;
  EXPECT_CALL(*mockComponent, getPos()).WillOnce(Return(V3D{1, 1, 1}));

  CowPtr<Component> contents(mockComponent);
  Node node(contents);

  node.const_ref().getPos();
  EXPECT_TRUE(Mock::VerifyAndClear(&mockComponent));
}

TEST(node_test, test_construction_with_parent) {

  Node_sptr parent =
      std::make_shared<Node>(CowPtr<Component>(new MockComponent));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent, contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_FALSE(node.hasChildren());
}

TEST(node_test, test_construction_with_parent_and_child) {

  Node_sptr parent =
      std::make_shared<Node>(CowPtr<Component>(new MockComponent));
  Node_sptr child =
      std::make_shared<Node>(CowPtr<Component>(new MockComponent));
  CowPtr<Component> contents(new MockComponent);

  Node node(parent, child, contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_TRUE(node.hasChildren());
}

TEST(node_test, test_modify_linear) {

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

  // Sanity check newTree. We expect all nodes to be different.
  EXPECT_NE(newTree->root().get(), a.get()); // compare a to a`
  EXPECT_NE(newTree->root()->child(0).get(),
            a->child(0).get()); // compare b to b`
  EXPECT_NE(newTree->root()->child(0)->child(0).get(),
            a->child(0)->child(0).get()); // compare c to c`

  // However, only the contents of one component should be different.
  EXPECT_EQ(&newTree->root()->const_ref(), &a->const_ref());
  EXPECT_EQ(&newTree->root()->child(0)->const_ref(), &b->const_ref());
  EXPECT_NE(&newTree->root()->child(0)->child(0)->const_ref(),
            &c->const_ref());

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(c_contents));

  delete a_contents;
  delete b_contents;
  delete c_contents;
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

  MockComponent *a_contents = new MockComponent;
  EXPECT_CALL(*a_contents, equals(_)).WillOnce(Return(false));
  MockComponent *b_contents = new MockComponent;
  EXPECT_CALL(*b_contents, equals(_)).WillOnce(Return(false));
  MockComponent *c_contents = new MockComponent;
  EXPECT_CALL(*c_contents, equals(_)).WillOnce(Return(true));

  auto a = std::make_shared<Node>(CowPtr<Component>(a_contents));
  auto b = std::make_shared<Node>(a, CowPtr<Component>(b_contents));
  auto c = std::make_shared<Node>(a, CowPtr<Component>(c_contents));
  a->addChild(b);
  a->addChild(c);

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
  EXPECT_CALL(*c_contents, clone()).WillOnce(Return(new MockComponent));

  auto newTree = c->modify(command);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));

  // Sanity check newTree. We expect all nodes to be different.
  EXPECT_NE(newTree->root().get(), a.get()); // compare a to a`
  EXPECT_NE(newTree->root()->child(0).get(),
            a->child(0).get()); // compare b to b`
  EXPECT_NE(newTree->root()->child(1).get(),
            a->child(1).get()); // compare c to c`

  // However, only the contents of one component should be different.
  EXPECT_EQ(&newTree->root()->const_ref(), &a->const_ref());
  EXPECT_EQ(&newTree->root()->child(0)->const_ref(), &b->const_ref());
  EXPECT_NE(&newTree->root()->child(1)->const_ref(), &c->const_ref());

  delete a_contents;
  delete b_contents;
  delete c_contents;
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

  MockComponent *a_contents = new MockComponent;
  EXPECT_CALL(*a_contents, equals(_)).WillOnce(Return(false));
  MockComponent *b_contents = new MockComponent;
  EXPECT_CALL(*b_contents, equals(_)).WillOnce(Return(false));
  MockComponent *c_contents = new MockComponent;
  EXPECT_CALL(*c_contents, equals(_)).WillOnce(Return(true)); // To force an update of c
  MockComponent *d_contents = new MockComponent;
  EXPECT_CALL(*d_contents, equals(_)).WillOnce(Return(false)); // Will force and update even though not explicitly requested

  auto a = std::make_shared<Node>(CowPtr<Component>(a_contents));
  auto b = std::make_shared<Node>(a, CowPtr<Component>(b_contents));
  auto c = std::make_shared<Node>(a, CowPtr<Component>(c_contents));
  auto d = std::make_shared<Node>(c, CowPtr<Component>(d_contents));
  a->addChild(b);
  a->addChild(c);
  c->addChild(d);

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
   * but also on d_contents, as we have to cascade all changes during the wright*/
  MockCommmand command;
  EXPECT_CALL(command, execute(_)).Times(2);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone()).WillOnce(Return(new MockComponent));
  EXPECT_CALL(*d_contents, clone()).WillOnce(Return(new MockComponent));

  auto newTree = c->modify(command);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));

  // Sanity check newTree. We expect all nodes to be different.
  EXPECT_NE(newTree->root().get(), a.get()); // compare a to a`
  EXPECT_NE(newTree->root()->child(0).get(),
            a->child(0).get()); // compare b to b`
  EXPECT_NE(newTree->root()->child(1).get(),
            a->child(1).get()); // compare c to c`
  EXPECT_NE(newTree->root()->child(1)->child(0).get(),
            a->child(1)->child(0).get()); // compare d to d`

  // These contents should be the same
  EXPECT_EQ(&newTree->root()->const_ref(), &a->const_ref());
  EXPECT_EQ(&newTree->root()->child(0)->const_ref(), &b->const_ref());
  // These should not
  EXPECT_NE(&newTree->root()->child(1)->const_ref(), &c->const_ref());
  EXPECT_NE(&newTree->root()->child(1)->child(0)->const_ref(), &d->const_ref());

  delete a_contents;
  delete b_contents;
  delete c_contents;
  delete d_contents;
}

}
