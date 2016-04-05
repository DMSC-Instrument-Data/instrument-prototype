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

  auto a =
      Node_uptr(new Node(CowPtr<Component>(new NiceMock<MockComponent>())));

  // Calls std::shared_ptr<T>(std::unique_ptr<T>&&) constructor
  InstrumentTree instrument(std::move(a), 0);

  EXPECT_EQ(0, instrument.version());
}

TEST(instrument_tree_test, test_root_node_must_be_valid) {

  EXPECT_THROW(InstrumentTree(Node_const_uptr(nullptr), 0),
               std::invalid_argument);
}

TEST(instrument_tree_test, test_version_check_on_constructor) {

  const unsigned int versionNumber = 1;
  auto a = Node_uptr(new Node(CowPtr<Component>(new NiceMock<MockComponent>()),
                              versionNumber));
  auto b = Node_uptr(
      new Node(a.get(), CowPtr<Component>(new NiceMock<MockComponent>()),
               versionNumber + 1 /*version number incremented. This is bad*/));
  a->addChild(std::move(b));

  EXPECT_THROW(InstrumentTree(std::move(a), 0), std::invalid_argument);
}

TEST(instrument_tree_test, test_copy) {

  /*
   * Create a simple tree with two nodes
   InstrumentTree a ----> b
   */

  const unsigned int versionNumber = 1;
  auto a = Node_uptr(new Node(CowPtr<Component>(new NiceMock<MockComponent>()),
                              versionNumber));

  auto b = Node_uptr(new Node(a.get(),
                              CowPtr<Component>(new NiceMock<MockComponent>()),
                              versionNumber));

  a->addChild(std::move(b));

  InstrumentTree original(std::move(a), 0);

  // Perform copy.
  auto copy = original;

  // Check that the version numbers of the nodes are the same.
  EXPECT_EQ(copy.root().version(), original.root().version());
  EXPECT_EQ(copy.root().child(0).version(), original.root().child(0).version());

  auto itOrig = original.iterator();
  auto itCopy = copy.iterator();

  do {
    auto nodeOrig = itOrig->next();
    auto nodeCopy = itCopy->next();

    EXPECT_NE(nodeOrig, nodeCopy) << "All Nodes should be copies";

    EXPECT_EQ(&nodeOrig->const_ref(), &nodeCopy->const_ref())
        << "But underlying components should be the same objects upon copy";
  } while (!itOrig->atEnd() && !itCopy->atEnd());
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

  Node_uptr a(new Node(CowPtr<Component>(a_contents)));
  Node_uptr b(new Node(a.get(), CowPtr<Component>(b_contents)));
  Node_uptr c(new Node(a.get(), CowPtr<Component>(c_contents)));
  a->addChild(std::move(b));
  a->addChild(std::move(c));

  InstrumentTree instrument(std::move(a), 0);
  EXPECT_EQ(&instrument.root().const_ref(), a_contents);
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

  Node_uptr a(new Node(CowPtr<Component>(new NiceMock<MockComponent>())));
  DetectorIdType detector1Id(0);
  auto composite = std::make_shared<CompositeComponent>(ComponentIdType(1));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(detector1Id), V3D{1, 1, 1})));

  Node_uptr b(new Node(a.get(), CowPtr<Component>(composite)));

  DetectorIdType detector2Id = detector1Id + 1;
  Node_uptr c(new Node(
      a.get(),
      CowPtr<Component>(std::unique_ptr<DetectorComponent>(
                            new DetectorComponent(ComponentIdType(1),
                                                  DetectorIdType(detector2Id),
                                                  V3D{1, 1, 1})).release())));

  a->addChild(std::move(b));
  a->addChild(std::move(c));

  InstrumentTree tree(std::move(a), 2);

  const Detector &det1 = tree.getDetector(1);
  EXPECT_EQ(det1.detectorId(), detector1Id);

  const Detector &det2 = tree.getDetector(0);
  EXPECT_EQ(det2.detectorId(), detector2Id);

  // Ask for something that doesn't exist.
  EXPECT_THROW(tree.getDetector(3), std::invalid_argument);
}

TEST(instrument_tree_test, test_fill_map) {
  auto a =
      Node_uptr(new Node(CowPtr<Component>(new NiceMock<MockComponent>())));

  InstrumentTree instrument(std::move(a), 0);

  std::map<DetectorIdType, size_t> container;
  EXPECT_THROW(instrument.fillDetectorMap(container), std::runtime_error)
      << "Characterize that this has not been implemented yet";
}
}
