#include "CompositeComponent.h"
#include "PointSample.h"
#include "PointSource.h"
#include "DetectorComponent.h"
#include "DetectorInfo.h"
#include "FlatTree.h"
#include "MockTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <stdexcept>
#include <memory>
#include "IdType.h"
#include "SourceSampleDetectorPathFactory.h"

namespace {

TEST(detector_info_test, test_construct) {

  using namespace testing;
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(1));
  EXPECT_CALL(*pMockInstrumentTree, detectorComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>(1, 0)));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillRepeatedly(testing::Return(new Paths(1, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillRepeatedly(testing::Return(new Paths(1, Path{0})));

  DetectorInfoWithMockInstrument detectorInfo(mockInstrumentTree,
                                              mockPathFactory);

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
  EXPECT_TRUE(testing::Mock::VerifyAndClear(&mockPathFactory))
      << "PathFactory used incorrectly";
}

TEST(detector_info_test, test_construct_with_bad_l2_paths_throws) {

  using namespace testing;

  const size_t nDetectors = 1;

  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(nDetectors));
  EXPECT_CALL(*pMockInstrumentTree, detectorComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>(1, 0)));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};

  MockPathFactory mockPathFactory;
  // L1 is OK, contains source and sample possibilities.
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(1, Path{0, 0})));
  // Empty path is illegal for l2 calculations.
  Path emptyPath(0);
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(1, emptyPath)));

  EXPECT_THROW(
      DetectorInfoWithMockInstrument(mockInstrumentTree, mockPathFactory),
      std::logic_error);
}

TEST(detector_info_test, test_construct_with_bad_l1_paths_throws) {

  using namespace testing;

  const size_t nDetectors = 1;

  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(nDetectors));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};

  MockPathFactory mockPathFactory;
  // Single path entry for single detector. Not OK. L1 needs at least source +
  // sample paths defined.
  Path singleEntryPath(1, 0);
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(1, singleEntryPath)));
  // Single path entry for single detector. Fine for L2 calculations.
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(1, Path(1, 0))));

  EXPECT_THROW(
      DetectorInfoWithMockInstrument(mockInstrumentTree, mockPathFactory),
      std::logic_error);
}

TEST(detector_info_test, test_masking) {

  size_t nDetectors = 3;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0})));

  DetectorInfoWithNiceMockInstrument detectorInfo(
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      mockPathFactory);

  EXPECT_FALSE(detectorInfo.isMasked(0));
  detectorInfo.setMasked(0);
  EXPECT_TRUE(detectorInfo.isMasked(0));

  EXPECT_THROW(detectorInfo.setMasked(nDetectors), std::out_of_range);
  EXPECT_THROW(detectorInfo.isMasked(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_get_l2s) {

  size_t nDetectors = 3;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0})));

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      mockPathFactory};

  auto l2s = detectorInfo.l2s();

  EXPECT_EQ(l2s->size(), nDetectors);
}

TEST(detector_info_test, test_monitors) {

  size_t nDetectors = 3;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0})));

  DetectorInfoWithMockInstrument detectorInfo{
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      mockPathFactory};

  EXPECT_FALSE(detectorInfo.isMonitor(0));
  detectorInfo.setMonitor(0);
  EXPECT_TRUE(detectorInfo.isMonitor(0));

  EXPECT_THROW(detectorInfo.setMonitor(nDetectors), std::out_of_range);
  EXPECT_THROW(detectorInfo.isMonitor(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_calculate_l2_throw_out_of_range) {

  size_t nDetectors = 1;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0})));

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      mockPathFactory};

  EXPECT_THROW(detectorInfo.l2(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_calculate_l2) {

  using namespace testing;
  size_t nDetectors = 1;

  auto *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>(nDetectors);

  /*
    3-component instrument.

    Source at x=0
    Sample at x = 20
    Single detector at x=40
  */
  EXPECT_CALL(*pMockInstrumentTree, pathLengths())
      .WillRepeatedly(Return(std::vector<double>(2, 0)));
  EXPECT_CALL(*pMockInstrumentTree, startPositions())
      .WillRepeatedly(Return(
          std::vector<Eigen::Vector3d>{{0, 0, 0}, {0, 0, 20}, {0, 0, 40}}));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints())
      .WillRepeatedly(
          Return(std::vector<Eigen::Vector3d>{{0, 0, 0}, {0, 0, 20}}));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints())
      .WillRepeatedly(
          Return(std::vector<Eigen::Vector3d>{{0, 0, 0}, {0, 0, 20}}));
  EXPECT_CALL(*pMockInstrumentTree, sourcePathIndex())
      .WillRepeatedly(Return(0));
  EXPECT_CALL(*pMockInstrumentTree, samplePathIndex())
      .WillRepeatedly(Return(1));
  EXPECT_CALL(*pMockInstrumentTree, detIndexToCompIndex(_))
      .WillRepeatedly(Return(2));

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};

  auto l2 = detectorInfo.l2(0);
  EXPECT_EQ(l2, 20) << "sqrt((40 - 20)^2)";
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(pMockInstrumentTree));
}

TEST(detector_info_test, test_calculate_l1) {
  using namespace testing;
  size_t nDetectors = 1;

  auto *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>(nDetectors);

  /*
    Effectively a 2-component instrument.

    Source at x=0
    Sample at x = 20
  */
  EXPECT_CALL(*pMockInstrumentTree, pathLengths()).WillRepeatedly(Return(std::vector<double>(2,0)));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints()).WillRepeatedly(Return(std::vector<Eigen::Vector3d>{{0,0,3}, {0,0,5}}));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints()).WillRepeatedly(Return(std::vector<Eigen::Vector3d>{{0,0,3}, {0,0,5}}));
  EXPECT_CALL(*pMockInstrumentTree, sourcePathIndex())
      .WillRepeatedly(testing::Return(0));
  EXPECT_CALL(*pMockInstrumentTree, samplePathIndex())
      .WillRepeatedly(testing::Return(1));

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};

  auto l1 = detectorInfo.l1(0);
  EXPECT_EQ(l1, 2) << "sqrt((5 - 3)^2)";
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(pMockInstrumentTree));
}

TEST(detector_info_test, test_copy) {
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};
  // Set it up so that it has a 2-detector meta-data size.
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(2));
  EXPECT_CALL(*pMockInstrumentTree, detectorComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>{1, 2}));

  DetectorInfoWithMockInstrument original{
      std::shared_ptr<MockFlatTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

  // Set some arbitrary meta-data properties
  original.setMasked(0);
  original.setMonitor(1);

  // Perform the copy
  auto copy = original;

  EXPECT_EQ(copy.isMasked(0), original.isMasked(0));
  EXPECT_EQ(copy.isMasked(1), original.isMasked(1));
  EXPECT_EQ(copy.isMonitor(0), original.isMonitor(0));
  EXPECT_EQ(copy.isMonitor(1), original.isMonitor(1));

  // Instrument tree should be shared. (shared_ptr)
  EXPECT_EQ(pMockInstrumentTree, &copy.const_instrumentTree());
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree));
}

std::shared_ptr<FlatTree> makeInstrumentTree() {
  /*

        A
        |
 -----------------
 |      |    |   |
 B      C    D   E
                 |
                 F


  */

  auto a = std::make_shared<CompositeComponent>(ComponentIdType(1));

  // Add single detector (B)
  a->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      ComponentIdType(2), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));

  // Add single detector (C)
  a->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      ComponentIdType(2), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));

  // Add point source (D)
  a->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{-1, 0, 0}, ComponentIdType(3))));

  // Add arbitrary composite (E)
  auto d = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(4)));

  // Add point sample (F)
  d->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0.1, 0, 0}, ComponentIdType(5))));

  a->addComponent(std::move(d));

  return std::make_shared<FlatTree>(a);
}

TEST(detector_info_test,
     test_scanning_construction_throws_with_wrong_positions_size) {

  auto scanTimes = ScanTimes{ScanTime(0, 10), ScanTime(10, 20)}; // 2 scan times

  auto timeIndexes = std::vector<std::vector<size_t>>{{0, 2}, {1, 3}};

  auto rotations = std::vector<Eigen::Quaterniond>(
      4, Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()});

  // Too many positions!
  auto positions = std::vector<Eigen::Vector3d>(5); // Should have 4 positions
  // EXPECT_THROW(DetectorInfo<FlatTree>(makeInstrumentTree(), timeIndexes,
  // scanTimes, positions, rotations), std::invalid_argument) << "Should throw.
  // Too many positions";

  // Too many positions!
  positions = std::vector<Eigen::Vector3d>(3); // Should have 4 positions
  // EXPECT_THROW(DetectorInfo<FlatTree>(makeInstrumentTree(), timeIndexes,
  // scanTimes, positions, rotations), std::invalid_argument) << "Should throw.
  // Too few positions";
}

TEST(detector_info_test,
     test_scanning_construction_throws_with_wrong_scan_number) {

  auto rotations = std::vector<Eigen::Quaterniond>(
      4, Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()});

  auto positions = std::vector<Eigen::Vector3d>(4);

  auto timeIndexes = std::vector<std::vector<size_t>>{
      {0, 2}, {1, 3, 4}}; // Should only provide 4 time indexes

  auto scanTimes =
      ScanTimes{ScanTime(0, 10)}; // Only 1 scan time. This is wrong.

  EXPECT_THROW(DetectorInfo<FlatTree>(makeInstrumentTree(), timeIndexes,
                                      scanTimes, positions, rotations),
               std::invalid_argument)
      << "Should throw. Not enough scan times.";
}

TEST(detector_info_test,
     test_scanning_construction_throws_with_wrong_rotations_size) {

  auto scanTimes = ScanTimes{ScanTime(0, 10), ScanTime(10, 20)}; // 2 scan times

  auto timeIndexes = std::vector<std::vector<size_t>>{{0, 2}, {1, 3}};

  auto positions = std::vector<Eigen::Vector3d>(4);

  // Too many rotations
  auto rotations = std::vector<Eigen::Quaterniond>(
      5,
      Eigen::Quaterniond{
          Eigen::Affine3d::Identity().rotation()}); // Should have 4 rotations
  EXPECT_THROW(DetectorInfo<FlatTree>(makeInstrumentTree(), timeIndexes,
                                      scanTimes, positions, rotations),
               std::invalid_argument)
      << "Should throw. Too many rotations";

  // Too few rotations
  rotations = std::vector<Eigen::Quaterniond>(
      3,
      Eigen::Quaterniond{
          Eigen::Affine3d::Identity().rotation()}); // Should have 4 rotations
  EXPECT_THROW(DetectorInfo<FlatTree>(makeInstrumentTree(), timeIndexes,
                                      scanTimes, positions, rotations),
               std::invalid_argument)
      << "Should throw. Too few rotations";
}

TEST(detector_info_test, test_scanning_positions) {

  auto scanTimes = ScanTimes{ScanTime(0, 10), ScanTime(10, 20)}; // 2 scan times

  auto timeIndexes = std::vector<std::vector<size_t>>{{0, 2}, {1, 3}};

  auto positions = std::vector<Eigen::Vector3d>(4);
  positions[0] = Eigen::Vector3d{1, 0, 0};    // Detector B time 0
  positions[1] = Eigen::Vector3d{1, 2, 0};    // Detector C time 0
  positions[2] = Eigen::Vector3d{1.01, 0, 0}; // Detector B time 1
  positions[3] = Eigen::Vector3d{1.01, 2, 0}; // Detector C time 1

  auto rotations = std::vector<Eigen::Quaterniond>(
      4, Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()});

  DetectorInfo<FlatTree> detectorInfo(makeInstrumentTree(), timeIndexes,
                                      scanTimes, positions, rotations);

  EXPECT_EQ(positions[0],
            detectorInfo.position(0 /*detector index*/, 0 /*time index*/));
  EXPECT_EQ(positions[2],
            detectorInfo.position(0 /*detector index*/, 1 /*time index*/));
  EXPECT_EQ(positions[1],
            detectorInfo.position(1 /*detector index*/, 0 /*time index*/));
  EXPECT_EQ(positions[3],
            detectorInfo.position(1 /*detector index*/, 1 /*time index*/));
}

TEST(detector_info_test, test_move_scan_position) {

  auto scanTimes = ScanTimes{ScanTime(0, 10), ScanTime(10, 20)}; // 2 scan times

  auto timeIndexes = std::vector<std::vector<size_t>>{{0, 2}, {1, 3}};

  auto positions = std::vector<Eigen::Vector3d>(4);
  positions[0] = Eigen::Vector3d{1, 0, 0};    // Detector B time 0
  positions[1] = Eigen::Vector3d{1, 2, 0};    // Detector C time 0
  positions[2] = Eigen::Vector3d{1.01, 0, 0}; // Detector B time 1
  positions[3] = Eigen::Vector3d{1.01, 2, 0}; // Detector C time 1

  auto rotations = std::vector<Eigen::Quaterniond>(
      4, Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()});

  DetectorInfo<FlatTree> detectorInfo(makeInstrumentTree(), timeIndexes,
                                      scanTimes, positions, rotations);

  // We are going to move just Detector C time 1 by some offset.
  Eigen::Vector3d offset{-1, 0, 0};
  Eigen::Vector3d expected = positions[3] + offset;
  detectorInfo.moveDetector(1, 1, offset);

  // Check that detector position at this time scan was moved correctly
  Eigen::Vector3d actual = detectorInfo.position(1, 1);
  EXPECT_EQ(actual, expected);
}
}
