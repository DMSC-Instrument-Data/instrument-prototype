#include <gtest/gtest.h>
#include "AssemblyInfo.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "FlatTree.h"
#include "PointSample.h"
#include "PointSource.h"
#include <Eigen/Core>

namespace {

std::shared_ptr<FlatTree> makeInstrumentTree() {
  /*

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
      new PointSample(Eigen::Vector3d{0.1, 0, 0}, ComponentIdType(5))));

  a->addComponent(std::move(d));

  return std::make_shared<FlatTree>(a);
}

TEST(assembly_info_test, test_construction) {
  auto instrumentTree = makeInstrumentTree();
  auto componentInfo =
      std::make_shared<ComponentInfo<FlatTree>>(instrumentTree);
  AssemblyInfo<FlatTree> assemblyInfo{componentInfo};
}

TEST(assembly_info_test, test_positions) {
  auto instrumentTree = makeInstrumentTree();
  auto componentInfo =
      std::make_shared<ComponentInfo<FlatTree>>(instrumentTree);
  AssemblyInfo<FlatTree> assemblyInfo{componentInfo};

  EXPECT_TRUE(assemblyInfo.position(0).isApprox(componentInfo->position(0)))
      << "Assembly 0 is same as Component 0";
  EXPECT_TRUE(assemblyInfo.position(1).isApprox(componentInfo->position(3)))
      << "Assembly 1 is same as Component 3";
}

TEST(assembly_info_test, test_rotations) {
  auto instrumentTree = makeInstrumentTree();
  auto componentInfo =
      std::make_shared<ComponentInfo<FlatTree>>(instrumentTree);
  AssemblyInfo<FlatTree> assemblyInfo{componentInfo};

  EXPECT_TRUE(assemblyInfo.rotation(0).isApprox(componentInfo->rotation(0)))
      << "Assembly 0 is same as Component 0";
  EXPECT_TRUE(assemblyInfo.rotation(1).isApprox(componentInfo->rotation(3)))
      << "Assembly 1 is same as Component 3";
}
TEST(assembly_info_test, test_move) {

  auto instrumentTree = makeInstrumentTree();
  auto componentInfo =
      std::make_shared<ComponentInfo<FlatTree>>(instrumentTree);
  AssemblyInfo<FlatTree> assemblyInfo{componentInfo};

  auto currentRootPos = assemblyInfo.position(0); // root position
  auto currentLeafPos = componentInfo->position(1);
  Eigen::Vector3d delta{1, 0, 0};
  assemblyInfo.moveAssemblyComponent(0, delta);

  EXPECT_TRUE(assemblyInfo.position(0).isApprox(currentRootPos + delta))
      << "Test root moved";
  EXPECT_TRUE(componentInfo->position(1).isApprox(currentLeafPos + delta))
      << "Test leaf moved. Changes should be progated down.";
}
}
