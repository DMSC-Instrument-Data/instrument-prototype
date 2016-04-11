#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include "IdType.h"

namespace {

class PolymorphicInstrumentTree {
public:
  virtual void fillDetectorMap(const std::map<DetectorIdType, size_t> &toFill) const = 0;
  virtual ~PolymorphicInstrumentTree() {}
};

class MockInstrumentTree : public PolymorphicInstrumentTree {
public:
  virtual ~MockInstrumentTree() {}
  MOCK_CONST_METHOD1(fillDetectorMap, void(const std::map<DetectorIdType, size_t> &));
};

using DetectorInfoWithMockInstrument = DetectorInfo<MockInstrumentTree>;

TEST(detector_info_test, test_construct) {

  MockInstrumentTree* pMockInstrumentTree = new MockInstrumentTree;
  std::shared_ptr<MockInstrumentTree> mockInstrumentTree{pMockInstrumentTree};
  EXPECT_CALL(*pMockInstrumentTree, fillDetectorMap(testing::_)).Times(1);
  DetectorInfoWithMockInstrument detectorInfo{mockInstrumentTree};
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}
}
