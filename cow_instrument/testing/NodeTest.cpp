#include "Node.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(node_test, test_construction_isolated) {

  CowPtr<Component> contents(new MockComponent);
  const std::string nodeName = "node1";
  Node node(contents, nodeName);

  EXPECT_FALSE(node.hasParent());
  EXPECT_FALSE(node.hasChildren());
  EXPECT_EQ(node.version(), 0);
  EXPECT_EQ(node.name(), nodeName);
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

  CowPtr<Component> contents(new MockComponent);

  Node node(0, contents);

  EXPECT_TRUE(node.hasParent());
  EXPECT_FALSE(node.hasChildren());
}

TEST(node_test, test_construction_with_parent_and_child) {

  CowPtr<Component> contents(new MockComponent);

  Node node(0, contents);
  node.addChild(42);

  EXPECT_TRUE(node.hasParent());
  EXPECT_TRUE(node.hasChildren());
}

TEST(node_test, test_copy_constructor) {
  CowPtr<Component> contents(new MockComponent);

  Node node(42, contents);
  node.addChild(13);
  auto copy(node);

  EXPECT_EQ(copy.parent(), node.parent());
  EXPECT_EQ(copy.version(), node.version());
  EXPECT_EQ(copy.name(), node.name());
  EXPECT_EQ(copy.children(), node.children());
  EXPECT_EQ(&copy.const_ref(), &node.const_ref());
}
}
