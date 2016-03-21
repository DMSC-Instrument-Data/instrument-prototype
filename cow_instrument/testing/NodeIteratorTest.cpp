#include "NodeIterator.h"
#include "Node.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"

using namespace testing;

namespace {

TEST(node_iterator_test, node_construction){

    // We have something we want to iterate over
    Node node{CowPtr<Component>(new MockComponent)};
    // We create the iterator
    NodeIterator iterator{node};
    // Check that the iterator is fundamentally working off a copy
    EXPECT_NE(iterator.next(), &node);
}

TEST(node_iterator_test, node_uptr_construction){
    NodeIterator iterator{Node_uptr(new Node{CowPtr<Component>(new MockComponent)})};
    EXPECT_FALSE(iterator.atEnd());
}

TEST(node_iterator_test, simple_iterator) {

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

  Node_uptr a(new Node(CowPtr<Component>(a_contents)));
  Node_uptr b(new Node(a.get(), CowPtr<Component>(b_contents)));
  Node_uptr c(new Node(a.get(), CowPtr<Component>(c_contents)));
  a->addChild(std::move(b));
  a->addChild(std::move(c));

  NodeIterator iterator(std::move(a));

  iterator.next(); // stack implementation last first.
  iterator.next();
  iterator.next();
  auto null_result = iterator.next();
  EXPECT_TRUE(nullptr == null_result);
  EXPECT_TRUE(iterator.atEnd());
}

}
