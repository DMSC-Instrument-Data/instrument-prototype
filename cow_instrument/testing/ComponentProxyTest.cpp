#include "ComponentProxy.h"
#include "NullComponent.h"
#include "MockTypes.h"
#include <gtest/gtest.h>

TEST(component_proxy_test, test_root_construction) {
  auto mockComponent = new MockComponent{};
  ComponentProxy proxy{mockComponent};
  EXPECT_FALSE(proxy.hasParent());
  EXPECT_FALSE(proxy.hasChildren());
  EXPECT_TRUE(dynamic_cast<const MockComponent *>(&proxy.const_ref()) !=
              nullptr);
  delete mockComponent;
}

TEST(component_proxy_test, test_add_leaf_constructor) {
  auto mockComponent = new MockComponent{};
  ComponentProxy proxy{0, mockComponent};
  EXPECT_TRUE(proxy.hasParent());
  EXPECT_FALSE(proxy.hasChildren());
  EXPECT_EQ(proxy.parent(), 0);
  delete mockComponent;
}

TEST(component_proxy_test, test_merge_trees_constructor) {
  auto mockComponent = new MockComponent{};
  ComponentProxy proxy{0, mockComponent, std::vector<size_t>(1, 2)};
  EXPECT_TRUE(proxy.hasParent());
  EXPECT_TRUE(proxy.hasChildren());
  EXPECT_EQ(std::vector<size_t>(1, 2), proxy.children());
  EXPECT_EQ(proxy.parent(), 0);
  delete mockComponent;
}

TEST(component_proxy_test, test_add_children) {
  auto mockComponent = new MockComponent{};
  ComponentProxy proxy{mockComponent};
  proxy.addChild(1);
  EXPECT_TRUE(proxy.hasChildren());
  EXPECT_EQ(proxy.nChildren(), 1);
  EXPECT_EQ(std::vector<size_t>(1, 1), proxy.children());
  delete mockComponent;
}
