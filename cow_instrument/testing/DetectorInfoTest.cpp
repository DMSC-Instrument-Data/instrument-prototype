#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <stdexcept>
#include "IdType.h"

namespace {

class PolymorphicInstrumentTree {
public:
  virtual size_t nDetectors() const = 0;
  virtual ~PolymorphicInstrumentTree() {}
};

class MockInstrumentTree : public PolymorphicInstrumentTree {
public:
  MockInstrumentTree() {}
  MockInstrumentTree(size_t nDetectors) {
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(nDetectors));
  }
  MOCK_CONST_METHOD0(nDetectors, size_t());
  virtual ~MockInstrumentTree() {}
};

using DetectorInfoWithMockInstrument = DetectorInfo<MockInstrumentTree>;

TEST(detector_info_test, test_construct) {

  MockInstrumentTree *pMockInstrumentTree = new MockInstrumentTree;
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(10));

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
}
