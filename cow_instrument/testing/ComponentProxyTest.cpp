#include "ComponentProxy.h"
#include <gtest/gtest.h>

TEST(component_proxy_test, test_root_construction) {
  ComponentIdType compId(1);
  ComponentProxy proxy{compId};
  EXPECT_FALSE(proxy.hasParent());
  EXPECT_FALSE(proxy.hasChildren());
}

TEST(component_proxy_test, test_add_leaf_constructor) {
  ComponentIdType compId(1);
  ComponentProxy proxy{0, compId};
  EXPECT_TRUE(proxy.hasParent());
  EXPECT_FALSE(proxy.hasChildren());
  EXPECT_EQ(proxy.parent(), 0);
}

TEST(component_proxy_test, test_merge_trees_constructor) {
  ComponentIdType compId(1);
  ComponentProxy proxy{0, compId, std::vector<size_t>(1, 2)};
  EXPECT_TRUE(proxy.hasParent());
  EXPECT_TRUE(proxy.hasChildren());
  EXPECT_EQ(std::vector<size_t>(1, 2), proxy.children());
  EXPECT_EQ(proxy.parent(), 0);
}

TEST(component_proxy_test, test_add_children) {
  ComponentIdType compId(1);
  ComponentProxy proxy{compId};
  proxy.addChild(1);
  EXPECT_TRUE(proxy.hasChildren());
  EXPECT_EQ(proxy.nChildren(), 1);
  EXPECT_EQ(std::vector<size_t>(1, 1), proxy.children());
}

TEST(component_proxy_test, test_equals) {
  ComponentIdType idA(1);
  ComponentIdType idB(1);

  ComponentProxy proxyA{0, idA, std::vector<size_t>(2, 2)};
  ComponentProxy proxyB{0, idB, std::vector<size_t>(2, 2)};

  EXPECT_EQ(proxyA, proxyB);
}

TEST(component_proxy_test, test_not_equals_when_components_not_equal) {
  ComponentIdType idA(1);
  ComponentIdType idB(2);
  ComponentProxy proxyA{0, idA, std::vector<size_t>(2, 2)};
  ComponentProxy proxyB{0, idB, std::vector<size_t>(2, 2)};

  EXPECT_NE(proxyA, proxyB) << "Components not the same";
}

TEST(component_proxy_test, test_not_equals_when_parents_not_equals) {

  ComponentIdType idA(1);
  ComponentIdType idB(1);

  ComponentProxy proxyA{10, idA, std::vector<size_t>(2, 2)};
  ComponentProxy proxyB{0, idB, std::vector<size_t>(2, 2)};

  EXPECT_NE(proxyA, proxyB) << "Parent indexes not the same";
}

TEST(component_proxy_test, test_not_equals_when_children_not_equals) {
  ComponentIdType idA(1);
  ComponentIdType idB(1);

  ComponentProxy proxyA{0, idA, std::vector<size_t>(2, 3)};
  ComponentProxy proxyB{0, idB, std::vector<size_t>(2, 2)};

  EXPECT_NE(proxyA, proxyB) << "Child indexes not the same";
}
