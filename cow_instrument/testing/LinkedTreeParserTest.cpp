#include <gtest/gtest.h>
#include <memory>
#include <set>
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "LinkedTreeParser.h"
#include "PointSample.h"
#include "PointSource.h"

namespace {

std::shared_ptr<CompositeComponent> makeTree() {
  /*

    we start like this. A-B-C-D are components

        A
        |
 ------------------
 |                |
 B -              E
 |  |
 C  D



  */

  // Create B
  auto b = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(2)));
  // Add C to B
  b->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 0}, ComponentIdType(3))));
  // Add D to B
  b->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      ComponentIdType(4), DetectorIdType(1), Eigen::Vector3d{0, 0, 0})));

  // Add B to A
  auto a = std::make_shared<CompositeComponent>(ComponentIdType(1));
  a->addComponent(std::move(b));

  // Add E to A
  a->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{-1, 0, 0}, ComponentIdType(4))));

  return a;
}

TEST(linked_tree_parser_test, test_construction) {
  LinkedTreeParser componentInfo;
}

TEST(linked_tree_parser_test, test_rotations) {

  auto comp = makeTree();
  LinkedTreeParser info;
  comp->registerContents(info);
  auto rotations = info.startRotations();
  EXPECT_EQ(rotations.size(), info.componentSize());
}

TEST(linked_tree_parser_test, test_start_entry_exit_points) {
  auto comp = makeTree();
  LinkedTreeParser info;
  comp->registerContents(info);
  auto pathIndexes = info.pathComponentIndexes();
  auto positions = info.startPositions();
  auto exitPoints = info.startExitPoints();
  auto entryPoints = info.startEntryPoints();
  auto lengths = info.pathLengths();

  // For point path components. We expect exit and entry positions to be the
  // same as point position.
  size_t i = 0;
  for (auto &index : pathIndexes) {

    EXPECT_EQ(positions[index], exitPoints[i]);
    EXPECT_EQ(positions[index], entryPoints[i]);
    EXPECT_EQ(lengths[i], 0);
    ++i;
  }
}

TEST(linked_tree_parser_test, test_component_ids) {

  auto comp = makeTree();
  LinkedTreeParser info;
  comp->registerContents(info);
  auto vecComponentIds = info.componentIds();
  auto componentIds =
      std::set<ComponentIdType>(vecComponentIds.begin(), vecComponentIds.end());
  EXPECT_EQ(componentIds.count(ComponentIdType(1)), 1)
      << "Should have on Id matching this";
  EXPECT_EQ(componentIds.count(ComponentIdType(2)), 1)
      << "Should have on Id matching this";
  EXPECT_EQ(componentIds.count(ComponentIdType(3)), 1)
      << "Should have on Id matching this";
  EXPECT_EQ(componentIds.count(ComponentIdType(4)), 1)
      << "Should have on Id matching this";
  EXPECT_EQ(componentIds.count(ComponentIdType(5)), 0)
      << "Should NOT have on Id matching this";
}

TEST(linked_tree_parser_test, test_indexing) {

  auto comp = makeTree();
  LinkedTreeParser info;
  comp->registerContents(info);

  auto branchIndexes = info.branchNodeComponentIndexes();
  auto pathIndexes = info.pathComponentIndexes();
  auto detectorIndexes = info.detectorComponentIndexes();

  EXPECT_EQ(branchIndexes.size(), 2) << "Made from two composite components";
  EXPECT_EQ(pathIndexes.size(), 2) << "Made from two path components";
  EXPECT_EQ(detectorIndexes.size(), 1)
      << "Made from one detector path component";

  EXPECT_EQ(branchIndexes[0], 0);
  EXPECT_EQ(branchIndexes[1], 1);
  EXPECT_EQ(pathIndexes[0], 2);
  EXPECT_EQ(detectorIndexes[0], 3);
  EXPECT_EQ(pathIndexes[1], 4);
}
}
