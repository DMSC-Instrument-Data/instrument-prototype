#include "ComponentInfo.h"
#include <gtest/gtest.h>
#include <memory>
#include <set>
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "PointSample.h"
#include "PointSource.h"

namespace {

TEST(component_info_test, test_construction) { ComponentInfo componentInfo; }

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

TEST(component_info_test, test_component_proxies) {

  auto comp = makeTree();

  ComponentInfo info;
  comp->registerContents(info);
  auto it = info.begin();

  // Check the first component A.
  EXPECT_EQ(it->componentId(), ComponentIdType(1));
  EXPECT_FALSE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->nChildren(), 2);
  EXPECT_EQ(it->children(), (std::vector<size_t>{1, 3}));
  // Move on to B
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(2));
  EXPECT_TRUE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->nChildren(), 1);
  EXPECT_EQ(it->parent(), 0);
  EXPECT_EQ(it->children(), (std::vector<size_t>{2}));
  // Move on to C
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(3));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 1);
  // Move on to D
  ++it;
  EXPECT_EQ(it->componentId(), ComponentIdType(4));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 0);
}

TEST(component_info_test, test_equals) {
  ComponentInfo a;
  ComponentInfo b;

  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);

  // Fill our componentinfos with the same info.
  auto comp = makeTree();
  comp->registerContents(a);
  comp->registerContents(b);

  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);
}

TEST(component_info_test, test_equals_when_component_pointers_are_different) {
  ComponentInfo a;
  ComponentInfo b;
  auto compA = makeTree();
  auto compB = compA->clone();
  compA->registerContents(a);
  compB->registerContents(b);

  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);
}

TEST(component_info_test, test_subtree_search) {

  auto comp = makeTree();
  ComponentInfo info;
  comp->registerContents(info);

  // Subtree of A
  auto indexes = info.subTreeIndexes(0);
  EXPECT_EQ(std::set<size_t>(indexes.begin(), indexes.end()),
            (std::set<size_t>{0, 1, 2, 3}))
      << "Subtree for A. should include everything";

  // Subtree of B
  indexes = info.subTreeIndexes(1);
  EXPECT_EQ(std::set<size_t>(indexes.begin(), indexes.end()),
            (std::set<size_t>{1, 2}));

  // Subtree of C
  indexes = info.subTreeIndexes(2);
  EXPECT_EQ(std::set<size_t>(indexes.begin(), indexes.end()),
            (std::set<size_t>{2}))
      << "Should just include itself. Subtree for C incorrect";

  // Subtree of D
  indexes = info.subTreeIndexes(3);
  EXPECT_EQ(std::set<size_t>(indexes.begin(), indexes.end()),
            (std::set<size_t>{3}))
      << "Should just include itself. Subtree for D incorrect";
}

TEST(component_info_test, test_rotations) {

  auto comp = makeTree();
  ComponentInfo info;
  comp->registerContents(info);
  auto rotations = info.startRotations();
  EXPECT_EQ(rotations.size(), info.componentSize());
}

TEST(component_info_test, test_positions) {

  auto comp = makeTree();
  ComponentInfo info;
  comp->registerContents(info);
  auto start = info.begin();
  auto last = --info.end();
  auto positions = info.startPositions();
  EXPECT_EQ(positions.size(), info.componentSize());
  EXPECT_EQ(positions.front(), comp->getPos());
  EXPECT_EQ(positions.back(), comp->children()[1]->getPos());
}

TEST(component_info_test, test_start_entry_exit_points) {
  auto comp = makeTree();
  ComponentInfo info;
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

TEST(component_info_test, test_component_ids) {

  auto comp = makeTree();
  ComponentInfo info;
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

TEST(component_info_test, test_fill_component_map) {

  auto comp = makeTree();
  ComponentInfo info;
  comp->registerContents(info);

  std::map<ComponentIdType, std::vector<size_t>> compIdMap;
  info.fillComponentMap(compIdMap);

  auto allCompIds = info.componentIds();
  size_t compIndex = 0;
  for (auto &id : allCompIds) {
    auto componentIndexesAtId = compIdMap[id];
    EXPECT_EQ(componentIndexesAtId.size(), 1) << "Should find 1 of everything";
    EXPECT_EQ(componentIndexesAtId[0], compIndex) << "Component index mismatch";
    ++compIndex;
  }
}

TEST(component_info_test, test_fill_component_map_scanning) {

  auto scan1 = makeTree();
  auto scan2 = makeTree();
  ComponentInfo info;
  scan1->registerContents(info);
  scan2->registerContents(info);

  std::map<ComponentIdType, std::vector<size_t>> compIdMap;
  info.fillComponentMap(compIdMap);

  auto allCompIds = info.componentIds();
  for (auto &id : allCompIds) {
    auto componentIndexesAtId = compIdMap[id];
    EXPECT_EQ(componentIndexesAtId.size(), 2) << "Should be 2 of everything";
  }
}

TEST(component_info_test, test_fill_detector_map) {

  auto comp = makeTree();
  ComponentInfo info;
  comp->registerContents(info);

  std::map<DetectorIdType, std::vector<size_t>> detectorIdMap;
  info.fillDetectorMap(detectorIdMap);

  auto allDetectorIds = info.detectorIds();
  size_t detectorIndex = 0;
  for (auto &id : allDetectorIds) {
    auto detectorIndexesAtId = detectorIdMap[id];
    EXPECT_EQ(detectorIndexesAtId.size(), 1) << "Should find 1 of everything";
    EXPECT_EQ(detectorIndexesAtId[0], detectorIndex)
        << "Detector index mismatch";
    ++detectorIndex;
  }
}

TEST(component_info_test, test_fill_detector_map_scanning) {

  auto scan1 = makeTree();
  auto scan2 = makeTree();
  ComponentInfo info;
  scan1->registerContents(info);
  scan2->registerContents(info);

  std::map<DetectorIdType, std::vector<size_t>> detectorIdMap;
  info.fillDetectorMap(detectorIdMap);

  auto allDetectorIds = info.detectorIds();
  for (auto &id : allDetectorIds) {
    auto detectorIndexesAtId = detectorIdMap[id];
    EXPECT_EQ(detectorIndexesAtId.size(), 2) << "Should be 2 of everything";
  }
}
}
