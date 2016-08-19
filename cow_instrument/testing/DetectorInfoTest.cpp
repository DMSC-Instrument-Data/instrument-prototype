#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "MockTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <stdexcept>
#include <memory>
#include "IdType.h"
#include "Command.h"
#include "SourceSampleDetectorPathFactory.h"

namespace {

void addMockSourceSampleToInstrument(
    testing::NiceMock<MockInstrumentTree> *pMockInstrumentTree,
    MockPathComponent &source, MockPathComponent &sample,
    Eigen::Vector3d sourcePos = Eigen::Vector3d{0, 0, 0}, Eigen::Vector3d samplePos = Eigen::Vector3d{0, 0, 20}) {
  testing::Action<double()> returnZeroLength = testing::Return(0.0);

  // This is where I place the source
  testing::Action<Eigen::Vector3d()> returnSourceCentre = testing::Return(sourcePos);
  EXPECT_CALL(source, length()).WillRepeatedly(returnZeroLength);
  EXPECT_CALL(source, entryPoint()).WillRepeatedly(returnSourceCentre);
  EXPECT_CALL(source, exitPoint()).WillRepeatedly(returnSourceCentre);

  // This is where I place the sample
  testing::Action<Eigen::Vector3d()> returnSampleCentre = testing::Return(samplePos);
  EXPECT_CALL(sample, length()).WillRepeatedly(returnZeroLength);
  EXPECT_CALL(sample, entryPoint()).WillRepeatedly(returnSampleCentre);
  EXPECT_CALL(sample, exitPoint()).WillRepeatedly(returnSampleCentre);

  const size_t sourceIndex = 0;
  const size_t sampleIndex = 1;

  // Hook it all up to the instrument tree
  EXPECT_CALL(*pMockInstrumentTree, sourcePathIndex())
      .WillRepeatedly(testing::Return(sourceIndex));
  EXPECT_CALL(*pMockInstrumentTree, samplePathIndex())
      .WillRepeatedly(testing::Return(sampleIndex));
  EXPECT_CALL(*pMockInstrumentTree, getPathComponent(sourceIndex))
      .WillRepeatedly(testing::ReturnRef(source));
  EXPECT_CALL(*pMockInstrumentTree, getPathComponent(sampleIndex))
      .WillRepeatedly(testing::ReturnRef(sample));
}

TEST(detector_info_test, test_construct) {

  using namespace testing;
  MockInstrumentTree *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(1));
  NiceMock<MockDetector> mockDetector;
  EXPECT_CALL(*pMockInstrumentTree, getDetector(_))
      .Times(1)
      .WillOnce(ReturnRef(mockDetector));

  std::shared_ptr<MockInstrumentTree> mockInstrumentTree{pMockInstrumentTree};

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

  MockInstrumentTree *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(nDetectors));

  NiceMock<MockDetector> mockDetector;
  EXPECT_CALL(*pMockInstrumentTree, getDetector(_))
      .Times(1)
      .WillOnce(ReturnRef(mockDetector));

  std::shared_ptr<MockInstrumentTree> mockInstrumentTree{pMockInstrumentTree};

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

  MockInstrumentTree *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(nDetectors));

  std::shared_ptr<MockInstrumentTree> mockInstrumentTree{pMockInstrumentTree};

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
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors),
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
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors),
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
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors),
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
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors),
      mockPathFactory};

  EXPECT_THROW(detectorInfo.l2(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_calculate_l2) {

  size_t nDetectors = 1;

  MockDetector detector;
  MockPathComponent source;
  MockPathComponent sample;

  // This is where I place the detector
  EXPECT_CALL(detector, getPos())
      .WillRepeatedly(testing::Return(Eigen::Vector3d{0, 0, 40}));

  auto *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>(nDetectors);

  EXPECT_CALL(*pMockInstrumentTree, getDetector(testing::_))
      .WillRepeatedly(testing::ReturnRef(detector));

  addMockSourceSampleToInstrument(pMockInstrumentTree, source, sample);

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};

  auto l2 = detectorInfo.l2(0);
  EXPECT_EQ(l2, 20) << "sqrt((40 - 20)^2)";
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(&source));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(&sample));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(&detector));
}

TEST(detector_info_test, test_calculate_l1) {

  size_t nDetectors = 1;

  MockDetector detector;
  MockPathComponent source;
  MockPathComponent sample;

  // This is where I place the detector
  EXPECT_CALL(detector, getPos())
      .WillRepeatedly(testing::Return(Eigen::Vector3d{0, 0, 40}));

  auto *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>(nDetectors);

  EXPECT_CALL(*pMockInstrumentTree, getDetector(testing::_))
      .WillRepeatedly(testing::ReturnRef(detector));

  addMockSourceSampleToInstrument(pMockInstrumentTree, source, sample,
                                  Eigen::Vector3d{0, 0, 3}, Eigen::Vector3d{0, 0, 5});

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};

  auto l1 = detectorInfo.l1(0);
  EXPECT_EQ(l1, 2) << "sqrt((5 - 3)^2)";
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(&source));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(&sample));
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(&detector));
}

TEST(detector_info_test, test_modify) {

  using namespace testing;
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};

  // We expect that the modify method of the existing instrument tree gets
  // called
  EXPECT_CALL(*pMockInstrumentTree, modifyProxy(testing::_, testing::_))
      .Times(1);

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<MockInstrumentTree>{}};

  MockCommand command;
  detectorInfo.modify(0, command);

  // test modify called on instrument.
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree));
}

TEST(detector_info_test, test_copy) {
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};
  // Set it up so that it has a 2-detector meta-data size.
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(2));

  DetectorInfoWithMockInstrument original{
      std::shared_ptr<MockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<MockInstrumentTree>{}};

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
