#include <gtest/gtest.h>
#include "InstrumentTree.h"
#include "CompositeComponent.h"
#include "MockTypes.h"
#include "SourceSampleDetectorPathFactory.h"
#include "InstrumentTree.h"
#include "DetectorComponent.h"
#include "PointSample.h"
#include "PointSource.h"
#include <Eigen/Core>
#include <memory>

InstrumentTree
make_very_basic_tree(ComponentIdType idForSource = ComponentIdType(0),
                     ComponentIdType idForSample = ComponentIdType(1),
                     ComponentIdType idForDetector1 = ComponentIdType(2),
                     ComponentIdType idForDetector2 = ComponentIdType(3)) {

  /*

        Root
        |
 ---------------------------------------------------------
 |                   |               |                   |
 B (Detector1)   C (Detector2)   C (Source)          D (Sample)

  */

  auto root = std::make_shared<CompositeComponent>(ComponentIdType(0));
  root->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      idForDetector1, DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));

  root->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      idForDetector2, DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));

  root->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{0, 0, 0}, idForSource)));
  root->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0, 0, 10}, idForSample)));

  return InstrumentTree(root);
}

TEST(source_sample_detector_path_factory_test, test_l1_paths) {

  auto instrument = make_very_basic_tree();
  SourceSampleDetectorPathFactory<InstrumentTree> pathFactory{};
  auto *paths = pathFactory.createL1(instrument);
  EXPECT_EQ(paths->size(), instrument.nDetectors())
      << "Wrong number of L1 paths";
  EXPECT_EQ((*paths)[0][0], instrument.sourcePathIndex());
  EXPECT_EQ((*paths)[0][1], instrument.samplePathIndex());
  EXPECT_EQ((*paths)[0].size(), 2) << "Should have only 2 path entries for L1";
}

TEST(source_sample_detector_path_factory_test, test_l2_paths) {

  auto instrument = make_very_basic_tree();
  SourceSampleDetectorPathFactory<InstrumentTree> pathFactory{};
  auto *paths = pathFactory.createL2(instrument);
  EXPECT_EQ(paths->size(), instrument.nDetectors())
      << "Wrong number of L2 paths";
  EXPECT_EQ((*paths)[0][0], instrument.samplePathIndex());
  EXPECT_EQ((*paths)[0].size(), 1) << "Should have only 1 path entries for L2. "
                                      "Detector indexes are not counted";
}
