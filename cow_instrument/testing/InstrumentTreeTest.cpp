#include "InstrumentTree.h"
#include "NodeIterator.h"
#include "Node.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"
#include "DetectorComponent.h"
#include "CompositeComponent.h"

using namespace testing;

namespace {

TEST(instrument_tree_test, test_uptr_constructor) {

    auto a = Node_uptr(new Node(CowPtr<Component>(new NiceMock<MockComponent>())));

    // Calls std::shared_ptr<T>(std::unique_ptr<T>&&) constructor
    InstrumentTree instrument(std::move(a));

}

TEST(instrument_tree_test, test_constructor) {

  /*

        A
        |
 ------------------
 |                |
 B                C

  */

  MockComponent *a_contents = new NiceMock<MockComponent>();
  MockComponent *b_contents = new NiceMock<MockComponent>();
  MockComponent *c_contents = new NiceMock<MockComponent>();

  auto a = std::make_shared<Node>(CowPtr<Component>(a_contents));
  auto b = std::make_shared<Node>(a, CowPtr<Component>(b_contents));
  auto c = std::make_shared<Node>(a, CowPtr<Component>(c_contents));
  a->addChild(b);
  a->addChild(c);

  InstrumentTree instrument(a);
  EXPECT_EQ(instrument.root().get(), a.get());
  EXPECT_FALSE(instrument.iterator()->atEnd());
}

TEST(instrument_tree_test, test_detector_access) {

  /*

        A (not a detector)
        |
 -------------------------------------------------
 |                                               |
 B (Composite containing Detector)               C (Detector)

  */

  auto a = std::make_shared<Node>(CowPtr<Component>(new NiceMock<MockComponent>()));

  size_t detector1Id = 1;
  CompositeComponent_sptr composite = std::make_shared<CompositeComponent>();
  composite->addComponent(
      std::make_shared<DetectorComponent>(detector1Id, V3D{1, 1, 1}));
  auto b = std::make_shared<Node>(a, CowPtr<Component>(composite));

  size_t detector2Id = detector1Id + 1;
  auto c = std::make_shared<Node>(
      a,
      CowPtr<Component>(std::make_shared<DetectorComponent>(detector2Id, V3D{2, 2, 2})));

  a->addChild(b);
  a->addChild(c);

  InstrumentTree tree(a);

  const Detector &det1 = tree.getDetector(detector1Id);
  EXPECT_EQ(det1.id(), detector1Id);

  const Detector &det2 = tree.getDetector(detector2Id);
  EXPECT_EQ(det2.id(), detector2Id);

  // Ask for something that doesn't exist.
  EXPECT_THROW(tree.getDetector(3), std::invalid_argument);
}
}
