#include "gtest/gtest.h"
#include "CompositeComponent.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(composite_component_test, test_construction){
    CompositeComponent composite{ComponentIdType(1)};
    EXPECT_EQ(0, composite.size());
}

TEST(composite_component_test, test_provide_name) {
  const std::string name = "bank_x";
  CompositeComponent composite{ComponentIdType(1), name};
  EXPECT_EQ(composite.name(), name);
}

TEST(composite_component_test, test_clone){
    CompositeComponent composite{ComponentIdType(1)};

    MockComponent* childA = new MockComponent;
    EXPECT_CALL(*childA, clone()).WillRepeatedly(Return(new MockComponent));
    MockComponent* childB = new MockComponent;
    EXPECT_CALL(*childB, clone()).WillRepeatedly(Return(new MockComponent));

    composite.addComponent(std::unique_ptr<MockComponent>(childA));
    composite.addComponent(std::unique_ptr<MockComponent>(childB));
    auto* clone = composite.clone();
    EXPECT_EQ(clone->size(), composite.size());
    delete clone;
    EXPECT_TRUE(Mock::VerifyAndClearExpectations(childA));
    EXPECT_TRUE(Mock::VerifyAndClearExpectations(childB));
}

TEST(composite_component_test, test_get_pos){

    MockComponent* childA = new MockComponent;
    EXPECT_CALL(*childA, getPos()).WillRepeatedly(Return(V3D{1,1,1}));
    MockComponent* childB = new MockComponent;
    EXPECT_CALL(*childB, getPos()).WillRepeatedly(Return(V3D{2,2,2}));
    MockComponent* childC = new MockComponent;
    EXPECT_CALL(*childC, getPos()).WillRepeatedly(Return(V3D{3,3,3}));

    CompositeComponent composite{ComponentIdType(1)};
    composite.addComponent(std::unique_ptr<MockComponent>(childA));
    composite.addComponent(std::unique_ptr<MockComponent>(childB));
    composite.addComponent(std::unique_ptr<MockComponent>(childC));

    V3D pos = composite.getPos();
    EXPECT_EQ(pos[0],2);
    EXPECT_EQ(pos[1],2);
    EXPECT_EQ(pos[2],2);
}






} // namespace
