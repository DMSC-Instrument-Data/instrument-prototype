#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "MockTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <stdexcept>
#include "IdType.h"

namespace {

class PolymorphicInstrumentTree {
public:
  virtual size_t nDetectors() const = 0;
  virtual V3D sourcePos() const = 0;
  virtual V3D samplePos() const = 0;
  virtual const Detector &getDetector(size_t detectorIndex) const = 0;
  virtual ~PolymorphicInstrumentTree() {}
};

class MockInstrumentTree : public PolymorphicInstrumentTree {
public:
  MockInstrumentTree() {
    ON_CALL(*this, samplePos()).WillByDefault(testing::Return(V3D{0, 0, 20}));
    ON_CALL(*this, sourcePos()).WillByDefault(testing::Return(V3D{0, 0, 0}));
  }
  MockInstrumentTree(size_t nDetectors) {
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(nDetectors));
    ON_CALL(*this, samplePos()).WillByDefault(testing::Return(V3D{0, 0, 20}));
    ON_CALL(*this, sourcePos()).WillByDefault(testing::Return(V3D{0, 0, 0}));
  }
  MOCK_CONST_METHOD0(nDetectors, size_t());
  MOCK_CONST_METHOD0(sourcePos, V3D());
  MOCK_CONST_METHOD0(samplePos, V3D());
  MOCK_CONST_METHOD1(getDetector, const Detector &(size_t));
  virtual ~MockInstrumentTree() {}
};

using NiceMockInstrumentTree = testing::NiceMock<MockInstrumentTree>;
using DetectorInfoWithMockInstrument = DetectorInfo<MockInstrumentTree>;
using DetectorInfoWithNiceMockInstrument = DetectorInfo<NiceMockInstrumentTree>;

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
}
