#include "NodeIterator.h"
#include "Node.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(node_iterator_test, simple_iterator){

    /*

          A
          |
   ------------------
   |                |
   B                C

    */

    MockComponent *a_contents = new MockComponent;
    MockComponent *b_contents = new MockComponent;
    MockComponent *c_contents = new MockComponent;

    auto a = std::make_shared<Node>(CowPtr<Component>(a_contents));
    auto b = std::make_shared<Node>(a, CowPtr<Component>(b_contents));
    auto c = std::make_shared<Node>(a, CowPtr<Component>(c_contents));
    a->addChild(b);
    a->addChild(c);


    NodeIterator iterator(a);


    EXPECT_EQ(iterator.next().get(), c.get()); // stack implementation last first.
    EXPECT_EQ(iterator.next().get(), b.get());
    EXPECT_EQ(iterator.next().get(), a.get());
    auto null_result = iterator.next();
    EXPECT_TRUE(nullptr == null_result.get());
    EXPECT_TRUE(iterator.atEnd());


    delete a_contents;
    delete b_contents;
    delete c_contents;

}

}
