#include "InstrumentTree.h"
#include "NodeIterator.h"
#include "Node.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(instrument_tree_test, test_constructor) {

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

  InstrumentTree instrument(a);
  EXPECT_EQ(instrument.root().get(), a.get());
  EXPECT_FALSE(instrument.iterator()->atEnd());
}
}
