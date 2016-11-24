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
  EXPECT_CALL(*pMockInstrumentTree, detIndexToCompIndex(_))
      .Times(1)
      .WillOnce(Return(0));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(1, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(1, Path{0})));

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
  EXPECT_CALL(*pMockInstrumentTree, detIndexToCompIndex(_))
      .Times(1)
      .WillOnce(Return(0));

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

TEST(detector_info_test, test_move) {

  using namespace testing;

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillOnce(Return(std::vector<size_t>{0}));

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

  auto before = detectorInfo.position(0);
  auto offset = Eigen::Vector3d{1, 0, 0};
  detectorInfo.move(0, offset);
  auto after = detectorInfo.position(0);

  EXPECT_EQ(after, before + offset);
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(instrumentTree));
}

TEST(detector_info_test, test_single_rotation_around_component_origin) {

  using namespace testing;

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillOnce(Return(std::vector<size_t>{0}));
  EXPECT_CALL(*instrumentTree, startPositions())
      .WillOnce(Return(std::vector<Eigen::Vector3d>{Eigen::Vector3d{0, 0, 0}}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillOnce(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

  const size_t sampleComponentIndex = 0;

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{detectorInfo.position(
      sampleComponentIndex)}; // rotate around component center

  detectorInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                      rotationCenter);

  auto after = detectorInfo.rotation(sampleComponentIndex);
  auto rotMatrix = after.toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal rotation not updated correctly";

  EXPECT_TRUE(detectorInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d{0, 0, 0}, 1e-14))
      << "Position should be unchanged as rotation was around its own center";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(instrumentTree));
}

TEST(detector_info_test, test_multiple_rotation_around_component_origin) {

  using namespace testing;

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 4;
  const Eigen::Vector3d rotationCenter{0, 0, 0};

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillRepeatedly(Return(std::vector<size_t>{0}));
  EXPECT_CALL(*instrumentTree, startPositions())
      .WillOnce(Return(std::vector<Eigen::Vector3d>{rotationCenter}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillOnce(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

  const size_t sampleComponentIndex = 0;

  // Rotate sample once by 45 degrees
  detectorInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                      rotationCenter);
  // Rotate sample again by 45 degrees
  detectorInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                      rotationCenter);

  auto samplePosition = detectorInfo.position(sampleComponentIndex);

  // Check that the position has not changed
  EXPECT_TRUE(samplePosition.isApprox(rotationCenter, 1e-14))
      << "Position should not change";

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix =
      detectorInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(detector_info_test, test_single_rotation_around_arbitrary_center) {

  using namespace testing;

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{0, 0, 0};
  const Eigen::Vector3d componentCenter{1, 0, 0};

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillRepeatedly(Return(std::vector<size_t>{0}));
  EXPECT_CALL(*instrumentTree, startPositions())
      .WillOnce(Return(std::vector<Eigen::Vector3d>{componentCenter}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillOnce(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};
  const size_t sampleComponentIndex = 0;

  detectorInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                      rotationCenter);
  // Check that the position has the rotation applied.
  EXPECT_TRUE(detectorInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d(0, 1, 0), 1e-14));

  // Check that the internal rotation gets updated
  Eigen::Matrix3d rotMatrix =
      detectorInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(detector_info_test, test_multiple_rotation_arbitrary_center) {

  using namespace testing;
  const Eigen::Vector3d rotationAxisZ{0, 0, 1};
  const Eigen::Vector3d rotationAxisX{1, 0, 0};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{0, 0, 0};
  const Eigen::Vector3d componentCenter{1, 0, 0};

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillRepeatedly(Return(std::vector<size_t>{0}));
  EXPECT_CALL(*instrumentTree, startPositions())
      .WillOnce(Return(std::vector<Eigen::Vector3d>{componentCenter}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillOnce(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};
  const size_t sampleComponentIndex = 0;

  // Rotate once by 90 degrees around z should put detector at 0,1,0
  detectorInfo.rotate(sampleComponentIndex, rotationAxisZ, rotationAngle,
                      rotationCenter);
  // Rotate again by 90 degrees around x should put detector at 0,0,1
  detectorInfo.rotate(sampleComponentIndex, rotationAxisX, rotationAngle,
                      rotationCenter);

  // Check that the position has the rotations applied.
  EXPECT_TRUE(detectorInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d(0, 0, 1), 1e-14));

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix =
      detectorInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 0, 1}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(detector_info_test, test_copy) {
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};
  // Set it up so that it has a 2-detector meta-data size.
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(2));

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

TEST(detector_info_test, test_component_duration_default) {

  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};

  DetectorInfoWithMockInstrument info{
      std::shared_ptr<MockFlatTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

  EXPECT_EQ(info.scanCount(), 1) << "Only one scan position by default";
  auto duration = info.componentDuration(0);
  EXPECT_EQ(duration.start(), std::time_t(0));

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree));
}


TEST(detector_info_test, test_scan_construction) {

  // Create an instrument with a source a sample and a detector
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors()).WillRepeatedly(testing::Return(1));
  EXPECT_CALL(*pMockInstrumentTree, samplePathIndex()).WillRepeatedly(testing::Return(size_t(0)));
  EXPECT_CALL(*pMockInstrumentTree, sourcePathIndex()).WillRepeatedly(testing::Return(size_t(1)));
  EXPECT_CALL(*pMockInstrumentTree, componentSize()).WillRepeatedly(testing::Return(3));
  EXPECT_CALL(*pMockInstrumentTree, startPositions())
      .WillRepeatedly(testing::Return(
          std::vector<Eigen::Vector3d>({{0, 0, 0}, {10, 0, 0}, {20, 0, 0}})));
  EXPECT_CALL(*pMockInstrumentTree,  startRotations())
      .WillRepeatedly(testing::Return(std::vector<Eigen::Quaterniond>(
          3,
          Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  EXPECT_CALL(*pMockInstrumentTree,  startEntryPoints())
      .WillRepeatedly(testing::Return(
          std::vector<Eigen::Vector3d>({{0, 0, 0}, {10, 0, 0}, {20, 0, 0}})));
  EXPECT_CALL(*pMockInstrumentTree,  startExitPoints())
      .WillRepeatedly(testing::Return(
          std::vector<Eigen::Vector3d>({{0, 0, 0}, {10, 0, 0}, {20, 0, 0}})));
  EXPECT_CALL(*pMockInstrumentTree,  pathLengths())
      .WillRepeatedly(testing::Return(
          std::vector<double>(3 /*componentSize()*/, 0)));

  auto timeIndexes = std::make_shared<std::vector<size_t>>();
  timeIndexes->push_back(0);
  timeIndexes->push_back(0);
  timeIndexes->push_back(1);
  auto scanTimes = std::make_shared<ScanTimes>();

  scanTimes->push_back(ScanTime{std::time_t(0), 100});
  scanTimes->push_back(ScanTime{std::time_t(50), 100});

  DetectorInfoWithMockInstrument info{
      std::shared_ptr<MockFlatTree>(pMockInstrumentTree),SourceSampleDetectorPathFactory<MockFlatTree>{}
      , timeIndexes, scanTimes};


  EXPECT_EQ(info.scanCount(), 2) << "Only two scan position by default";
  EXPECT_EQ(info.componentDuration(0).start(), std::time_t(0)) << "Wrong time domain for source";
  EXPECT_EQ(info.componentDuration(1).start(), std::time_t(0)) << "Wrong time domain for sample";
  EXPECT_EQ(info.componentDuration(2).start(), std::time_t(50)) << "Wrong time domain for detector";


  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree));
}

}
