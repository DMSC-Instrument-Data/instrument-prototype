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

TEST(component_proxy_test, test_equals) {
  using namespace testing;
  auto mockComponentA = new MockComponent{};
  auto mockComponentB = new MockComponent{};
  EXPECT_CALL(*mockComponentA, equals(_)).WillOnce(Return(true));
  ComponentProxy proxyA{0, mockComponentA, std::vector<size_t>(2, 2)};
  ComponentProxy proxyB{0, mockComponentB, std::vector<size_t>(2, 2)};

  EXPECT_EQ(proxyA, proxyB);

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentA));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentB));
  delete mockComponentA;
  delete mockComponentB;
}

TEST(component_proxy_test, test_not_equals_when_components_not_equal) {
  using namespace testing;
  auto mockComponentA = new MockComponent{};
  auto mockComponentB = new MockComponent{};
  EXPECT_CALL(*mockComponentA, equals(_)).WillOnce(Return(false));
  ComponentProxy proxyA{0, mockComponentA, std::vector<size_t>(2, 2)};
  ComponentProxy proxyB{0, mockComponentB, std::vector<size_t>(2, 2)};

  EXPECT_NE(proxyA, proxyB) << "Components not the same";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentA));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentB));

  delete mockComponentA;
  delete mockComponentB;
}

TEST(component_proxy_test, test_not_equals_when_parents_not_equals) {
  using namespace testing;
  auto mockComponentA = new MockComponent{};
  auto mockComponentB = new MockComponent{};
  EXPECT_CALL(*mockComponentA, equals(_)).WillOnce(Return(true));
  ComponentProxy proxyA{10, mockComponentA, std::vector<size_t>(2, 2)};
  ComponentProxy proxyB{0, mockComponentB, std::vector<size_t>(2, 2)};

  EXPECT_NE(proxyA, proxyB) << "Parent indexes not the same";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentA));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentB));
  delete mockComponentA;
  delete mockComponentB;
}

TEST(component_proxy_test, test_not_equals_when_children_not_equals) {
  using namespace testing;
  auto mockComponentA = new MockComponent{};
  auto mockComponentB = new MockComponent{};
  EXPECT_CALL(*mockComponentA, equals(_)).WillOnce(Return(true));
  ComponentProxy proxyA{0, mockComponentA, std::vector<size_t>(2, 3)};
  ComponentProxy proxyB{0, mockComponentB, std::vector<size_t>(2, 2)};

  EXPECT_NE(proxyA, proxyB) << "Child indexes not the same";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentA));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(mockComponentB));
  delete mockComponentA;
  delete mockComponentB;
}
