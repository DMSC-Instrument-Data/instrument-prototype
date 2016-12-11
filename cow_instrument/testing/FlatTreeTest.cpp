#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockTypes.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "LinkedTreeParser.h"
#include "ParabolicGuide.h"
#include "PointSource.h"
#include "PointSample.h"

using namespace testing;

namespace {

std::shared_ptr<CompositeComponent> make_component_tree() {
  /*

    we start like this. A-B-C-D are components

        A
        |
 ------------------
 |                |
 B                D
 |
 C



  */

  // Create B
  auto b = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(2)));
  // Add C to B
  b->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 0}, ComponentIdType(3))));

  // Add B to A
  auto a = std::make_shared<CompositeComponent>(ComponentIdType(1));
  a->addComponent(std::move(b));

  // Add D to A
  a->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{-1, 0, 0}, ComponentIdType(4))));

  return a;
}

FlatTree make_simple_tree(DetectorIdType detector1Id,
                          DetectorIdType detector2Id) {

  /*

        A (not a detector)
        |
 ---------------------------------------------------------------------
 |                                    |                 |            |
 B (Composite containing Detector)    C (Detector)    D (source)     E(Sample)

  */

  auto root = std::make_shared<CompositeComponent>(ComponentIdType(0));
  auto composite = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(1)));

  composite->addComponent(std::unique_ptr<DetectorComponent>(
      new DetectorComponent(ComponentIdType(1), DetectorIdType(detector1Id),
                            Eigen::Vector3d{1, 1, 1})));

  root->addComponent(std::move(composite));
  root->addComponent(std::unique_ptr<DetectorComponent>(
      new DetectorComponent(ComponentIdType(2), DetectorIdType(detector2Id),
                            Eigen::Vector3d{1, 1, 1})));

  root->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{0, 0, 0}, ComponentIdType(3))));
  root->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 10}, ComponentIdType(4))));

  return FlatTree(root);
}

FlatTree
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

  auto root = std::make_shared<CompositeComponent>(ComponentIdType(3));

  root->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      idForDetector, DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));

  root->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{0, 0, 0}, idForSource)));
  root->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 10}, idForSample)));

  return FlatTree(root);
}

/**
Test helper method. Adds a source and sample node to an existing root
and creates an instrument tree from it.
 */
FlatTree makeRegularInstrument(std::shared_ptr<CompositeComponent> &root) {

  root->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{0, 0, 0}, ComponentIdType(100))));
  root->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 10}, ComponentIdType(101))));

  return FlatTree(root);
}

TEST(instrument_tree_test, test_cannot_construct_without_sample) {

  auto source_ptr = new NiceMock<MockPathComponent>();
  auto source = std::shared_ptr<Component>(source_ptr);

  // Make the source
  EXPECT_CALL(*source_ptr, isSource()).WillRepeatedly(Return(true));
  EXPECT_CALL(*source_ptr, isSample()).WillRepeatedly(Return(false));

  EXPECT_THROW(FlatTree{source}, std::invalid_argument)
      << "Should throw, there is no sample";
}

TEST(instrument_tree_test, test_cannot_construct_without_source) {
  auto sample_ptr = new NiceMock<MockPathComponent>();
  auto sample = std::shared_ptr<Component>(sample_ptr);

  // Make a the sample
  EXPECT_CALL(*sample_ptr, isSource()).WillRepeatedly(Return(false));
  EXPECT_CALL(*sample_ptr, isSample()).WillRepeatedly(Return(true));
  EXPECT_THROW(FlatTree{sample}, std::invalid_argument)
      << "Should throw, there is not source";
}

TEST(instrument_tree_test, test_both_constructors) {

  auto source = make_component_tree();

  FlatTree treeA(source);

  LinkedTreeParser intermediate;
  source->registerContents(intermediate);
  auto proxies = intermediate.proxies();
  auto positions = intermediate.startPositions();
  auto rotations = intermediate.startRotations();
  auto componentIds = intermediate.componentIds();
  auto entryPoints = intermediate.startEntryPoints();
  auto exitPoints = intermediate.startExitPoints();
  auto pathLengths = intermediate.pathLengths();
  auto pathComponentIndexes = intermediate.pathComponentIndexes();
  auto detectorComponentIndexes = intermediate.detectorComponentIndexes();
  auto detectorIds = intermediate.detectorIds();

  FlatTree treeB(
      std::move(proxies), std::move(positions), std::move(rotations),
      std::move(componentIds), std::move(entryPoints), std::move(exitPoints),
      std::move(pathLengths), std::move(pathComponentIndexes),
      std::move(detectorComponentIndexes), std::move(detectorIds),
      intermediate.sourcePathIndex(), intermediate.samplePathIndex());

  EXPECT_EQ(treeA.startPositions(), treeB.startPositions())
      << "Unequal positions size";
  EXPECT_EQ(treeA.componentSize(), treeB.componentSize())
      << "Unequal component size";
  EXPECT_EQ(treeA.startEntryPoints(), treeB.startEntryPoints())
      << "Unequal entry points";
  EXPECT_EQ(treeA.startExitPoints(), treeB.startExitPoints())
      << "Unequal exit points";
  EXPECT_EQ(treeA.pathLengths(), treeB.pathLengths()) << "Unequal path lengths";

  EXPECT_EQ(treeA, treeB) << "Proxies not the same. Something badly wrong.";
}

TEST(instrument_tree_test, test_find_source_sample) {
  /*
   * One source, one sample, one detector
   */
  auto instrument = make_very_basic_tree();

  auto sourceProxy = instrument.proxyAt(instrument.sourceComponentIndex());
  auto sampleProxy = instrument.proxyAt(instrument.sampleComponentIndex());

  std::map<ComponentIdType, size_t> componentMap;
  instrument.fillComponentMap(componentMap);

  EXPECT_EQ(componentMap[sourceProxy.componentId()],
            instrument.sourceComponentIndex());
  EXPECT_EQ(componentMap[sampleProxy.componentId()],
            instrument.sampleComponentIndex());
}

TEST(instrument_tree_test, test_copy) {

  FlatTree original = make_simple_tree(DetectorIdType(1), DetectorIdType(2));

  // Perform copy.
  auto copy = original;

  auto itOrig = original.cbegin();
  auto itCopy = copy.cbegin();

  do {
    auto proxyOrig = (*itOrig++);
    auto proxyCopy = (*itCopy++);

    EXPECT_EQ(proxyOrig, proxyCopy);

  } while (itOrig != original.cend() && itCopy != copy.cend());
}

TEST(instrument_tree_test, test_fill_detector_map) {

  auto instrument = make_simple_tree(DetectorIdType(1), DetectorIdType(2));
  std::map<DetectorIdType, size_t> container;
  instrument.fillDetectorMap(container);
  EXPECT_EQ(container.size(), 2) << "Two detectors expected";
}

TEST(instrument_tree_test,
     test_get_detector_and_path_from_mixed_component_instrument) {

  const ComponentIdType idForSource(10);
  const ComponentIdType idForSample(20);
  const ComponentIdType idForDetector(30);

  auto instrument =
      make_very_basic_tree(idForSource, idForSample, idForDetector);

  std::map<ComponentIdType, size_t> componentIdMap;
  instrument.fillComponentMap(componentIdMap);
  EXPECT_EQ(componentIdMap[idForDetector], 1);
  EXPECT_EQ(componentIdMap[idForSource], 2);
  EXPECT_EQ(componentIdMap[idForSample], 3);
}

FlatTree makeInstrumentTree() {
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

  return FlatTree(a);
}

TEST(instrument_tree_test, test_component_proxies) {

  FlatTree instrument = makeInstrumentTree();
  auto it = instrument.begin();
  // Check the first component A.
  EXPECT_EQ(it->componentId(), ComponentIdType(1));
  EXPECT_FALSE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->nChildren(), 3);
  EXPECT_EQ(it->children(), (std::vector<size_t>{1, 2, 3}));
  // Move on to B
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(2));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
  // Move on to C
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(3));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
  // Move on to D
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(4));
  EXPECT_TRUE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
  EXPECT_EQ(it->children(), (std::vector<size_t>{4}));
  // Move on to E
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(5));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 3);
}

TEST(instrument_tree_test, test_subtree_search) {
  FlatTree instrument = makeInstrumentTree();

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

TEST(instrument_tree_test, test_positions) {

  auto comp = make_component_tree();
  FlatTree tree(comp);
  auto positions = tree.startPositions();
  EXPECT_EQ(positions.size(), tree.componentSize());
  EXPECT_EQ(positions.front(), comp->getPos());
  EXPECT_EQ(positions.back(), comp->children()[1]->getPos());
}

TEST(instrument_tree_test, test_equals) {
  auto comp = make_component_tree();
  FlatTree a(comp);
  FlatTree b(comp);

  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);

  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);
}

TEST(instrument_tree_test, test_equals_when_component_pointers_are_different) {

  std::shared_ptr<Component> compA = make_component_tree();
  std::shared_ptr<Component> compB(compA->clone());

  FlatTree a(compA);
  FlatTree b(compB);

  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);
}

TEST(instrument_tree_test, test_subtree_unreachable_throws) {

  FlatTree instrument = makeInstrumentTree();

  EXPECT_THROW(instrument.subTreeIndexes(instrument.componentSize()),
               std::invalid_argument);
}
}
