#include <gtest/gtest.h>
#include <memory>
#include <set>
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "LinkedTreeParser.h"
#include "PointSample.h"
#include "PointSource.h"

namespace {

TEST(soa_source_test, test_construction) { LinkedTreeParser componentInfo; }

std::shared_ptr<CompositeComponent> makeTree() {
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

TEST(soa_source_test, test_rotations) {

  auto comp = makeTree();
  LinkedTreeParser info;
  comp->registerContents(info);
  auto rotations = info.startRotations();
  EXPECT_EQ(rotations.size(), info.componentSize());
}

TEST(soa_source_test, test_start_entry_exit_points) {
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

TEST(soa_source_test, test_component_ids) {

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
}
