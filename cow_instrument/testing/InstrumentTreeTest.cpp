#include "InstrumentTree.h"
#include "Node.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"
#include "DetectorComponent.h"
#include "CompositeComponent.h"

using namespace testing;

namespace {

TEST(instrument_tree_test, test_uptr_constructor) {

  Node a(CowPtr<Component>(new NiceMock<MockComponent>()));

  // Calls std::shared_ptr<T>(std::unique_ptr<T>&&) constructor
  InstrumentTree instrument({a}, 0);

  EXPECT_EQ(0, instrument.version());
}

TEST(instrument_tree_test, test_root_node_must_be_valid) {

  EXPECT_THROW(InstrumentTree({}, 0), std::invalid_argument);
}

TEST(instrument_tree_test, test_version_check_on_constructor) {

  const unsigned int versionNumber = 1;
  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(new NiceMock<MockComponent>()), "a",
                     versionNumber);
  nodes.emplace_back(0, CowPtr<Component>(new NiceMock<MockComponent>()), "b",
                     versionNumber +
                         1 /*version number incremented. This is bad*/);
  nodes[0].addChild(1);

  EXPECT_THROW(InstrumentTree(std::move(nodes), 0), std::invalid_argument);
}

TEST(instrument_tree_test, test_copy) {

  /*
   * Create a simple tree with two nodes
   InstrumentTree a ----> b
   */

  const unsigned int versionNumber = 1;
  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(new NiceMock<MockComponent>()), "a",
                     versionNumber);
  nodes.emplace_back(0, CowPtr<Component>(new NiceMock<MockComponent>()), "b",
                     versionNumber);
  nodes[0].addChild(1);

  InstrumentTree original(std::move(nodes), 0);

  // Perform copy.
  auto copy = original;

  // Check that the version numbers of the nodes are the same.
  EXPECT_EQ(copy.root().version(), original.root().version());

  auto itOrig = original.cbegin();
  auto itCopy = copy.cbegin();

  do {
    auto nodeOrig = itOrig++;
    auto nodeCopy = itCopy++;

    EXPECT_NE(&(*nodeOrig), &(*nodeCopy)) << "All Nodes should be copies";
    EXPECT_EQ(nodeOrig->version(), nodeCopy->version());

    EXPECT_EQ(&nodeOrig->const_ref(), &nodeCopy->const_ref())
        << "But underlying components should be the same objects upon copy";
  } while (itOrig != original.cend() && itCopy != copy.cend());
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

  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(a_contents));
  nodes.emplace_back(0, CowPtr<Component>(b_contents));
  nodes.emplace_back(0, CowPtr<Component>(c_contents));
  nodes[0].addChild(1);
  nodes[0].addChild(2);

  InstrumentTree instrument(std::move(nodes), 0);
  EXPECT_EQ(&instrument.root().const_ref(), a_contents);
  EXPECT_NE(instrument.cbegin(), instrument.cend());
}

TEST(instrument_tree_test, test_detector_access) {

  /*

        A (not a detector)
        |
 -------------------------------------------------
 |                                               |
 B (Composite containing Detector)               C (Detector)

  */

  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(new NiceMock<MockComponent>()));
  DetectorIdType detector1Id(0);
  auto composite = std::make_shared<CompositeComponent>(ComponentIdType(1));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(detector1Id), V3D{1, 1, 1})));

  nodes.emplace_back(0, CowPtr<Component>(composite));

  DetectorIdType detector2Id = detector1Id + 1;
  nodes.emplace_back(
      0,
      CowPtr<Component>(std::unique_ptr<DetectorComponent>(
                            new DetectorComponent(ComponentIdType(1),
                                                  DetectorIdType(detector2Id),
                                                  V3D{1, 1, 1})).release()));

  nodes[0].addChild(1);
  nodes[0].addChild(2);

  InstrumentTree tree(std::move(nodes), 2);

  const Detector &det1 = tree.getDetector(0);
  EXPECT_EQ(det1.detectorId(), detector1Id);

  const Detector &det2 = tree.getDetector(1);
  EXPECT_EQ(det2.detectorId(), detector2Id);

  // Ask for something that doesn't exist.
  EXPECT_THROW(tree.getDetector(3), std::invalid_argument);
}

TEST(instrument_tree_test, test_fill_map) {
  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(new NiceMock<MockComponent>()));

  InstrumentTree instrument(std::move(nodes), 0);

  std::map<DetectorIdType, size_t> container;
  EXPECT_THROW(instrument.fillDetectorMap(container), std::runtime_error)
      << "Characterize that this has not been implemented yet";
}

TEST(instrument_tree_test, test_modify_linear) {

  /*
    we start like this
    a -> b -> c
  */

  auto *a_contents = new NiceMock<MockComponent>();
  auto *b_contents = new NiceMock<MockComponent>();
  auto *c_contents = new NiceMock<MockComponent>();

  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(a_contents));
  nodes.emplace_back(0, CowPtr<Component>(b_contents));
  nodes.emplace_back(1, CowPtr<Component>(c_contents));
  nodes[1].addChild(2);
  nodes[0].addChild(1);

  InstrumentTree instrument(std::move(nodes), 0);

  /*
    we then modify c.
    so we should now have a shallow copied node tree,

    a`-> b` -> |c`|
  */

  // We would therefore expect the command to be executed on c_contents
  NiceMock<MockCommand> command;
  EXPECT_CALL(command, execute(_)).Times(1);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  // Go from a to be to c, and modify that.
  auto newTree = instrument.modify(2, command);

  // Check that our tree has an incremented version
  EXPECT_EQ(newTree.version(), instrument.version() + 1);
  // And check further down in the tree too. The version should be the same
  // across the whole tree.
  EXPECT_EQ((newTree.begin() + 1)->version(), instrument.version() + 1);
  // And bottom of tree too. The version should be the same across the whole
  // tree.
  EXPECT_EQ((newTree.begin() + 2)->version(), instrument.version() + 1);

  // However, only the contents of one component should be different.
  EXPECT_EQ(&(newTree.begin() + 0)->const_ref(), a_contents);
  EXPECT_EQ(&(newTree.begin() + 1)->const_ref(), b_contents);
  EXPECT_NE(&(newTree.begin() + 2)->const_ref(), c_contents);

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(c_contents));
}

TEST(instrument_tree_test, test_modify_tree) {

  /*
    we start like this

        A
        |
 ------------------
 |                |
 B                C

  */

  MockComponent *a_contents = new NiceMock<MockComponent>();
  MockComponent *b_contents = new NiceMock<MockComponent>();
  MockComponent *c_contents = new NiceMock<MockComponent>();

  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(a_contents));
  nodes.emplace_back(0, CowPtr<Component>(b_contents));
  nodes.emplace_back(0, CowPtr<Component>(c_contents));
  nodes[0].addChild(1);
  nodes[0].addChild(2);

  InstrumentTree instrument(std::move(nodes), 0);

  /*
    we then modify c.
    so we should now have a shallow copied node tree,

        A`
        |
 ------------------
 |                |
 B`                |C`|
  */

  // We would therefore expect the command to be executed on c_contents
  NiceMock<MockCommand> command;
  EXPECT_CALL(command, execute(_)).Times(1);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  // Get to c via a, and modify c
  auto newTree = instrument.modify(2, command);

  // Check that our tree has an incremented version
  EXPECT_EQ(newTree.version(), instrument.version() + 1);
  // And check further down in the tree too. The version should be the same
  // across the whole tree.
  EXPECT_EQ((newTree.begin() + 1)->version(), instrument.version() + 1);
  EXPECT_EQ((newTree.begin() + 2)->version(), instrument.version() + 1);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));

  // However, only the contents of one component should be different.
  EXPECT_EQ(&(newTree.begin() + 0)->const_ref(), a_contents);
  EXPECT_EQ(&(newTree.begin() + 1)->const_ref(), b_contents);
  EXPECT_NE(&(newTree.begin() + 2)->const_ref(), c_contents);
}


TEST(node_test, test_tree_cascade) {

  /*
   We are making writes more expensive, but critically reads
   less expensive.

    we start like this

        A
        |
 ------------------
 |                |
 B                C
                  |
                  D


  */

  auto *a_contents = new NiceMock<MockComponent>();
  auto *b_contents = new NiceMock<MockComponent>();
  auto *c_contents = new NiceMock<MockComponent>();
  auto *d_contents = new NiceMock<MockComponent>();


  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(a_contents));
  nodes.emplace_back(0, CowPtr<Component>(b_contents));
  nodes.emplace_back(0, CowPtr<Component>(c_contents));
  nodes.emplace_back(2, CowPtr<Component>(d_contents));
  nodes[0].addChild(1);
  nodes[0].addChild(2);
  nodes[2].addChild(3);

  InstrumentTree instrument(std::move(nodes), 0);


  /*
    we then modify c.
    so we should now have a shallow copied node tree,

        A`
        |
 ------------------
 |                |
 B`                |C`|
                  |
                  |D`|

  */

  /* We would therefore expect the command to be executed on c_contents
   * but also on d_contents, as we have to cascade all changes during the
   * write*/
  NiceMock<MockCommand> command;
  EXPECT_CALL(command, execute(_)).Times(2);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));
  EXPECT_CALL(*d_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  auto newTree = instrument.modify(2, command);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(d_contents));

  // Check that our tree has an incremented version
  EXPECT_EQ(newTree.version(), instrument.version() + 1);
  // And check further down in the tree too. The version should be the same
  // across the whole tree.
  EXPECT_EQ((newTree.begin() + 1)->version(), instrument.version() + 1);
  EXPECT_EQ((newTree.begin() + 2)->version(), instrument.version() + 1);
  EXPECT_EQ((newTree.begin() + 3)->version(), instrument.version() + 1);

  // These contents should be the same
  EXPECT_EQ(&(newTree.begin() + 0)->const_ref(), a_contents);
  EXPECT_EQ(&(newTree.begin() + 1)->const_ref(), b_contents);
   // These should not
  EXPECT_NE(&(newTree.begin() + 2)->const_ref(), c_contents);
  EXPECT_NE(&(newTree.begin() + 3)->const_ref(), d_contents);
}

TEST(node_test, test_tree_no_cascade) {

  /*

    we start like this

        A
        |
 ------------------
 |                |
 B                C
                  |
                  D


  */

  auto *a_contents = new NiceMock<MockComponent>();
  auto *b_contents = new NiceMock<MockComponent>();
  auto *c_contents = new NiceMock<MockComponent>();
  auto *d_contents = new NiceMock<MockComponent>();

  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(a_contents));
  nodes.emplace_back(0, CowPtr<Component>(b_contents));
  nodes.emplace_back(0, CowPtr<Component>(c_contents));
  nodes.emplace_back(2, CowPtr<Component>(d_contents));
  nodes[0].addChild(1);
  nodes[0].addChild(2);
  nodes[2].addChild(3);

  InstrumentTree instrument(std::move(nodes), 0);


  /*
    we then modify c.
    so we should now have a shallow copied node tree.
    Note that D` is not deep copied.

        A`
        |
 ------------------
 |                |
 B`                |C`|
                  |
                  D`

  */

  /* We would therefore expect the command to be executed on c_contents
   * but NOT on d_contents, */
  NiceMock<MockCommand> command;
  // We make a command that is meta-data only (i.e. not cascading).
  EXPECT_CALL(command, isMetaDataCommand()).WillRepeatedly(Return(true));
  EXPECT_CALL(command, execute(_)).Times(1);

  // We would also expect that the contents of c are deep copied.
  EXPECT_CALL(*c_contents, clone())
      .WillOnce(Return(new NiceMock<MockComponent>()));

  auto newTree = instrument.modify(2, command);

  EXPECT_TRUE(Mock::VerifyAndClear(a_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(b_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(c_contents));
  EXPECT_TRUE(Mock::VerifyAndClear(d_contents));

  // Check that our tree has an incremented version
  EXPECT_EQ(newTree.version(), instrument.version() + 1);
  // And check further down in the tree too. The version should be the same
  // across the whole tree.
  EXPECT_EQ((newTree.begin() + 1)->version(), instrument.version() + 1);
  EXPECT_EQ((newTree.begin() + 2)->version(), instrument.version() + 1);
  EXPECT_EQ((newTree.begin() + 3)->version(), instrument.version() + 1);

  // These contents should be the same
  EXPECT_EQ(&(newTree.begin() + 0)->const_ref(), a_contents);
  EXPECT_EQ(&(newTree.begin() + 1)->const_ref(), b_contents);
  EXPECT_EQ(&(newTree.begin() + 3)->const_ref(), d_contents);
   // These should not
  EXPECT_NE(&(newTree.begin() + 2)->const_ref(), c_contents);

}


}
