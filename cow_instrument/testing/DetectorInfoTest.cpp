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

namespace {

TEST(detector_info_test, test_construct) {

  MockInstrumentTree *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(10));
  EXPECT_CALL(*pMockInstrumentTree, sourcePos()).Times(1);
  EXPECT_CALL(*pMockInstrumentTree, samplePos()).Times(1);

  std::shared_ptr<MockInstrumentTree> mockInstrumentTree{pMockInstrumentTree};

  DetectorInfoWithMockInstrument detectorInfo{mockInstrumentTree};
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}

TEST(detector_info_test, test_masking) {

  size_t nDetectors = 3;

  DetectorInfoWithMockInstrument detectorInfo{
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors)};

  EXPECT_FALSE(detectorInfo.isMasked(0));
  detectorInfo.setMasked(0);
  EXPECT_TRUE(detectorInfo.isMasked(0));

  EXPECT_THROW(detectorInfo.setMasked(nDetectors), std::out_of_range);
  EXPECT_THROW(detectorInfo.isMasked(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_monitors) {

  size_t nDetectors = 3;

  DetectorInfoWithMockInstrument detectorInfo{
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors)};

  EXPECT_FALSE(detectorInfo.isMonitor(0));
  detectorInfo.setMonitor(0);
  EXPECT_TRUE(detectorInfo.isMonitor(0));

  EXPECT_THROW(detectorInfo.setMonitor(nDetectors), std::out_of_range);
  EXPECT_THROW(detectorInfo.isMonitor(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_calculate_l2_throw_out_of_range) {

  size_t nDetectors = 1;

  DetectorInfoWithMockInstrument detectorInfo{
      std::make_shared<testing::NiceMock<MockInstrumentTree>>(nDetectors)};

  EXPECT_THROW(detectorInfo.l2(nDetectors), std::out_of_range);
}

TEST(detector_info_test, test_calculate_l2_calculate) {

  size_t nDetectors = 1;

  MockDetector detector;
  // This is where I place the detector
  EXPECT_CALL(detector, getPos())
      .WillRepeatedly(testing::Return(V3D{0, 0, 40}));

  auto *pMockInstrumentTree =
      new testing::NiceMock<MockInstrumentTree>(nDetectors);

  EXPECT_CALL(*pMockInstrumentTree, getDetector(testing::_))
      .WillRepeatedly(testing::ReturnRef(detector));
  // This is where I place the sample
  EXPECT_CALL(*pMockInstrumentTree, samplePos())
      .WillRepeatedly(testing::Return(V3D{0, 0, 20}));
  // This is where I place the source
  EXPECT_CALL(*pMockInstrumentTree, sourcePos())
      .WillRepeatedly(testing::Return(V3D{0, 0, 0}));

  DetectorInfoWithNiceMockInstrument detectorInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree)};

  auto l2 = detectorInfo.l2(0);
  EXPECT_EQ(l2, 20) << "sqrt((40 - 20)^2)";
}

TEST(detector_info_test, test_modify) {

  using namespace testing;
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};
  auto *pMockInstrumentTreeNew = new NiceMockInstrumentTree{};

  // We expect that the modify method of the existing instrument tree gets
  // called
  EXPECT_CALL(*pMockInstrumentTree, modifyProxy(testing::_))
      .Times(1)
      .WillOnce(Return(pMockInstrumentTreeNew));

  // We expect that the source position of the new instrument is required
  EXPECT_CALL(*pMockInstrumentTreeNew, sourcePos())
      .Times(1)
      .WillOnce(Return(V3D{0, 0, 0}));
  // We expect that the sample position of the new instrument is required
  EXPECT_CALL(*pMockInstrumentTreeNew, samplePos())
      .Times(1)
      .WillOnce(Return(V3D{1, 1, 1}));

  DetectorInfoWithMockInstrument detectorInfo{
      std::shared_ptr<MockInstrumentTree>(pMockInstrumentTree)};

  MockCommand command;
  detectorInfo.modify(0, command);

  // test modify called on instrument.
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree));
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTreeNew));
}

TEST(detector_info_test, test_copy) {
  auto *pMockInstrumentTree = new NiceMockInstrumentTree{};
  // Set it up so that it has a 2-detector meta-data size.
  EXPECT_CALL(*pMockInstrumentTree, nDetectors()).WillOnce(testing::Return(2));

  DetectorInfoWithMockInstrument original{
      std::move(std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree))};

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
