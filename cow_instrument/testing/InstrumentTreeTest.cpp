#include "InstrumentTree.h"
#include "Node.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"
#include "DetectorComponent.h"
#include "CompositeComponent.h"
#include "ParabolicGuide.h"
#include "NullComponent.h"
#include "PointSource.h"
#include "PointSample.h"

using namespace testing;

namespace {

InstrumentTree make_simple_tree(DetectorIdType detector1Id,
                                DetectorIdType detector2Id) {

  /*

        A (not a detector)
        |
 ---------------------------------------------------------------------
 |                                    |                 |            |
 B (Composite containing Detector)    C (Detector)    D (source)     E(Sample)

  */

  auto composite = std::make_shared<CompositeComponent>(ComponentIdType(1));
  composite->addComponent(
      std::unique_ptr<DetectorComponent>(new DetectorComponent(
          ComponentIdType(1), DetectorIdType(detector1Id), Eigen::Vector3d{1, 1, 1})));

  std::vector<Node> nodes;

  nodes.push_back(Node(CowPtr<Component>(new NiceMock<MockComponent>())));
  nodes.push_back(Node(0, CowPtr<Component>(composite)));
  nodes.push_back(Node(0, CowPtr<Component>(std::unique_ptr<DetectorComponent>(
                                                new DetectorComponent(
                                                    ComponentIdType(2),
                                                    DetectorIdType(detector2Id),
                                                    Eigen::Vector3d{1, 1, 1})).release())));

  nodes.push_back(Node(
      0, CowPtr<Component>(new PointSource(Eigen::Vector3d{0, 0, 0}, ComponentIdType(3)))));
  nodes.push_back(Node(0, CowPtr<Component>(new PointSample(
                              Eigen::Vector3d{0, 0, 10}, ComponentIdType(4)))));

  nodes[0].addChild(1); // add composite
  nodes[0].addChild(2); // add detector
  nodes[0].addChild(3); // add source
  nodes[0].addChild(4); // add sample

  return InstrumentTree(std::move(nodes));
}

InstrumentTree
make_very_basic_tree(ComponentIdType idForSource = ComponentIdType(0),
                     ComponentIdType idForSample = ComponentIdType(1),
                     ComponentIdType idForDetector = ComponentIdType(2)) {

  /*

        Root Node A
        |
 -------------------------------------------------
 |                            |                   |
 B (Detector)            C (Source)          D (Sample)

  */

  std::vector<Node> nodes;
  nodes.push_back(Node{});
  nodes.push_back(
      Node(0, CowPtr<Component>(new DetectorComponent(
                  idForDetector, DetectorIdType(1), Eigen::Vector3d{1, 1, 1}))));

  nodes.push_back(
      Node(0, CowPtr<Component>(new PointSource(Eigen::Vector3d{0, 0, 0}, idForSource))));
  nodes.push_back(
      Node(0, CowPtr<Component>(new PointSample(Eigen::Vector3d{0, 0, 10}, idForSample))));

  nodes[0].addChild(1);
  nodes[0].addChild(2);
  nodes[0].addChild(3);
  return InstrumentTree(std::move(nodes));
}

/**
Test helper method. Adds a source and sample node to an existing node vector
and creates an instrument tree from it.
 */
InstrumentTree makeRegularInstrument(std::vector<Node> &&nodes,
                                     int versionNumber = 0) {
  const size_t originalSize = nodes.size();
  nodes.push_back(Node(
      0, CowPtr<Component>(new PointSource(Eigen::Vector3d{0, 0, 0}, ComponentIdType(100))),
      "Source", versionNumber));
  nodes.push_back(Node(0, CowPtr<Component>(new PointSample(
                              Eigen::Vector3d{0, 0, 10}, ComponentIdType(101))),
                       "Sample", versionNumber));

  nodes[0].addChild(originalSize + 1);
  nodes[1].addChild(originalSize + 2);

  return InstrumentTree(std::move(nodes));
}

TEST(instrument_tree_test, test_uptr_constructor) {

  Node a(CowPtr<Component>(new NiceMock<MockComponent>()));

  // Calls std::shared_ptr<T>(std::unique_ptr<T>&&) constructor
  InstrumentTree instrument = makeRegularInstrument({a});

  EXPECT_EQ(0, instrument.version());
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

  EXPECT_THROW(makeRegularInstrument(std::move(nodes)), std::invalid_argument);
}

TEST(instrument_tree_test, test_cannot_construct_without_sample) {

  auto source_ptr = new NiceMock<MockPathComponent>();
  auto source = CowPtr<Component>(source_ptr);

  // Make a the source
  EXPECT_CALL(*source_ptr, isSource()).WillRepeatedly(Return(true));
  EXPECT_CALL(*source_ptr, isSample()).WillRepeatedly(Return(false));

  std::vector<Node> nodes;
  nodes.emplace_back(source);

  EXPECT_THROW(InstrumentTree(std::move(nodes)), std::invalid_argument)
      << "Should throw, there is no sample";
}

TEST(instrument_tree_test, test_cannot_construct_without_source) {
  auto sample_ptr = new NiceMock<MockPathComponent>();
  auto sample = CowPtr<Component>(sample_ptr);

  // Make a the source
  EXPECT_CALL(*sample_ptr, isSource()).WillRepeatedly(Return(false));
  EXPECT_CALL(*sample_ptr, isSample()).WillRepeatedly(Return(true));

  std::vector<Node> nodes;
  nodes.emplace_back(sample);

  EXPECT_THROW(InstrumentTree(std::move(nodes)), std::invalid_argument)
      << "Should throw, there is not source";
}

TEST(instrument_tree_test, test_find_source_sample) {
  /*
   * One source, one sample, one detector
   */
  auto instrument = make_very_basic_tree();

  const PathComponent &source = instrument.source();
  EXPECT_TRUE(dynamic_cast<const PointSource *>(&source) != NULL);
  const PathComponent &sample = instrument.sample();
  EXPECT_TRUE(dynamic_cast<const PointSample *>(&sample) != NULL);
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

  InstrumentTree original =
      makeRegularInstrument(std::move(nodes), versionNumber);

  // Perform copy.
  auto copy = original;

  // Check that the version numbers of the nodes are the same.
  EXPECT_EQ(copy.root().version(), original.root().version());

  auto itOrig = original.cbegin();
  auto itCopy = copy.cbegin();

  do {
    auto proxyOrig = (*itOrig++);
    auto proxyCopy = (*itCopy++);

    EXPECT_EQ(proxyOrig, proxyCopy);

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

  InstrumentTree instrument = makeRegularInstrument(std::move(nodes));
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
          ComponentIdType(1), DetectorIdType(detector1Id), Eigen::Vector3d{1, 1, 1})));

  nodes.emplace_back(0, CowPtr<Component>(composite));

  DetectorIdType detector2Id = detector1Id + 1;
  nodes.emplace_back(
      0,
      CowPtr<Component>(std::unique_ptr<DetectorComponent>(
                            new DetectorComponent(ComponentIdType(1),
                                                  DetectorIdType(detector2Id),
                                                  Eigen::Vector3d{1, 1, 1})).release()));

  nodes[0].addChild(1);
  nodes[0].addChild(2);

  InstrumentTree tree = makeRegularInstrument(std::move(nodes));

  const Detector &det1 = tree.getDetector(0);
  EXPECT_EQ(det1.detectorId(), detector1Id);

  const Detector &det2 = tree.getDetector(1);
  EXPECT_EQ(det2.detectorId(), detector2Id);

  // Ask for something that doesn't exist.
  EXPECT_THROW(tree.getDetector(3), std::invalid_argument);
}

TEST(instrument_tree_test, test_fill_detector_map_no_detectors) {
  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(new NiceMock<MockComponent>()));

  InstrumentTree instrument = makeRegularInstrument(std::move(nodes));

  std::map<DetectorIdType, size_t> container;
  instrument.fillDetectorMap(container);
  EXPECT_EQ(container.size(), 0) << "No detectors to add";
}

TEST(instrument_tree_test, test_fill_detector_map) {

  auto instrument = make_simple_tree(DetectorIdType(1), DetectorIdType(2));
  std::map<DetectorIdType, size_t> container;
  instrument.fillDetectorMap(container);
  EXPECT_EQ(container.size(), 2) << "Two detectors expected";
}

TEST(instrument_tree_test,
     test_get_detector_and_path_from_mixed_component_instrument) {

  const ComponentIdType idForSource(1);
  const ComponentIdType idForSample(2);
  const ComponentIdType idForDetector(3);

  auto instrument =
      make_very_basic_tree(idForSource, idForSample, idForDetector);
  const auto &detectorComponent = instrument.getDetector(0);
  const auto &pathComponent1 = instrument.getPathComponent(0);
  const auto &pathComponent2 = instrument.getPathComponent(1);

  EXPECT_EQ(detectorComponent.componentId(), idForDetector);
  EXPECT_EQ(pathComponent1.componentId(), idForSource);
  EXPECT_EQ(pathComponent2.componentId(), idForSample);
}

InstrumentTree makeInstrumentTree() {
  /*

    we start like this. A-B-C-D-E are components within a single node.

        A
        |
 ------------------------------
 |                |           |
 B                C           D
                              |
                              E


  */

  auto a = std::make_shared<CompositeComponent>(ComponentIdType(1));
  a->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      ComponentIdType(2), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));
  a->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{-1, 0, 0}, ComponentIdType(3))));

  auto d = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(4)));
  d->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 0}, ComponentIdType(5))));

  a->addComponent(std::move(d));

  return InstrumentTree(a);
}

TEST(instrument_tree_test, test_component_proxies) {

  InstrumentTree instrument = makeInstrumentTree();
  auto it = instrument.begin();
  // Check the first component A.
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(1));
  EXPECT_FALSE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->nChildren(), 3);
  EXPECT_EQ(it->children(), (std::vector<size_t>{1, 2, 3}));
  // Move on to B
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(2));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
  // Move on to C
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(3));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
  // Move on to D
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(4));
  EXPECT_TRUE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
  EXPECT_EQ(it->children(), (std::vector<size_t>{4}));
  // Move on to E
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(5));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 3);
}

TEST(instrument_tree_test, test_subtree_search) {
  InstrumentTree instrument = makeInstrumentTree();

  // Subtree of A
  auto indexes = instrument.subTreeIndexes(0);
  EXPECT_EQ(indexes, (std::vector<size_t>{0, 1, 2, 3, 4}))
      << "Subtree for A. should include everything";

  // Subtree of B
  indexes = instrument.subTreeIndexes(1);
  EXPECT_EQ(indexes, (std::vector<size_t>{1}))
      << "Should just include itself. Subtree for B incorrect";

  // Subtree of C
  indexes = instrument.subTreeIndexes(2);
  EXPECT_EQ(indexes, (std::vector<size_t>{2}))
      << "Should just include itself. Subtree for C incorrect";

  // Subtree of D
  indexes = instrument.subTreeIndexes(3);
  EXPECT_EQ(indexes, (std::vector<size_t>{3, 4})) << "Subtree for D incorrect";

  // Subtree of E
  indexes = instrument.subTreeIndexes(4);
  EXPECT_EQ(indexes, (std::vector<size_t>{4})) << "Subtree for E incorrect";
}

TEST(instrument_tree_test, test_subtree_unreachable_throws) {

  InstrumentTree instrument = makeInstrumentTree();

  EXPECT_THROW(instrument.subTreeIndexes(instrument.componentSize()),
               std::invalid_argument);
}
}
