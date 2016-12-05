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
      .WillRepeatedly(Return(0));

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

}
