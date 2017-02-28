#include <gtest/gtest.h>
#include "AssemblyInfo.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "FlatTree.h"
#include "PointSample.h"
#include "PointSource.h"
#include <Eigen/Core>

namespace {

std::shared_ptr<const FlatTree> makeInstrumentTree() {

  /*

        A
        |
 ------------------------------
 |                |           |
 B                C           D


  */

  auto a = std::make_shared<CompositeComponent>(ComponentIdType(1));
  a->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      ComponentIdType(2), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));
  a->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{-1, 0, 0}, ComponentIdType(3))));

  a->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0.1, 0, 0}, ComponentIdType(5))));

  return std::make_shared<const FlatTree>(a);
}

TEST(assembly_info_test, test_construction) {
  auto instrumentTree = makeInstrumentTree();
  auto componentInfo =
      std::make_shared<ComponentInfo<FlatTree>>(instrumentTree);
  AssemblyInfo<FlatTree> assemblyInfo{componentInfo};
}
}
