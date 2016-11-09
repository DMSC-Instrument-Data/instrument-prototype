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
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(1));
  EXPECT_FALSE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->nChildren(), 2);
  EXPECT_EQ(it->children(), (std::vector<size_t>{1, 3}));
  // Move on to B
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(2));
  EXPECT_TRUE(it->hasParent());
  EXPECT_TRUE(it->hasChildren());
  EXPECT_EQ(it->nChildren(), 1);
  EXPECT_EQ(it->parent(), 0);
  EXPECT_EQ(it->children(), (std::vector<size_t>{2}));
  // Move on to C
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(3));
  EXPECT_TRUE(it->hasParent());
  EXPECT_FALSE(it->hasChildren());
  EXPECT_EQ(it->parent(), 1);
  // Move on to D
  ++it;
  EXPECT_EQ(it->const_ref().componentId(), ComponentIdType(4));
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
  EXPECT_EQ(positions.front(), start->const_ref().getPos());
  EXPECT_EQ(positions.back(), last->const_ref().getPos());
}
}
