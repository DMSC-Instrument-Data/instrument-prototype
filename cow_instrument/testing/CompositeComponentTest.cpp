#include "gtest/gtest.h"
#include "CompositeComponent.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(composite_component_test, test_construction){
    CompositeComponent composite;
    EXPECT_EQ(0, composite.size());
}

TEST(composite_component_test, test_clone){
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
