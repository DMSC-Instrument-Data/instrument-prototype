
#include "FlatTree.h"
#include "MockTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>
#include <memory>
#include "PathComponentInfo.h"

namespace {

TEST(path_info_test, test_construct_simple) {

  using namespace testing;
  // Instrument with single path component. Nothing else.
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nPathComponents())
      .WillRepeatedly(testing::Return(1));
  EXPECT_CALL(*pMockInstrumentTree, startPositions())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {0.5, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startRotations())
      .WillRepeatedly(testing::Return(std::vector<Eigen::Quaterniond>(
          1, Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {0, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {1, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, pathComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>(1, 0)));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};
  PathComponentInfo<MockFlatTree> pathComponentInfo(mockInstrumentTree);

  EXPECT_EQ(pathComponentInfo.position(0), (Eigen::Vector3d{0.5, 0, 0}));
  EXPECT_EQ(pathComponentInfo.entryPoint(0), (Eigen::Vector3d{0, 0, 0}));
  EXPECT_EQ(pathComponentInfo.exitPoint(0), (Eigen::Vector3d{1, 0, 0}));

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}

TEST(path_info_test, test_construct_more_complex) {

  using namespace testing;
  // Instrument with two detectors and two path components.
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nPathComponents())
      .WillRepeatedly(testing::Return(2));

  // These are positions to be returned by the instrument (From all components).
  std::vector<Eigen::Vector3d> positions;
  positions.push_back({1, 0, 0});
  positions.push_back({2, 0, 0});
  positions.push_back({3, 0, 0});
  positions.push_back({4, 0, 0});

  EXPECT_CALL(*pMockInstrumentTree, startPositions())
      .WillRepeatedly(testing::Return(positions));
  EXPECT_CALL(*pMockInstrumentTree, startRotations())
      .WillRepeatedly(testing::Return(std::vector<Eigen::Quaterniond>(
          4, Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(4, {0, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(4, {0, 0, 0})));
  // Only index 2 and 3 of components are path components
  EXPECT_CALL(*pMockInstrumentTree, pathComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>({2, 3})));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};
  PathComponentInfo<MockFlatTree> pathComponentInfo(mockInstrumentTree);

  EXPECT_EQ(pathComponentInfo.position(0), (Eigen::Vector3d{3, 0, 0}));
  EXPECT_EQ(pathComponentInfo.position(1), (Eigen::Vector3d{4, 0, 0}));
  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}

TEST(path_info_test, test_out_of_bounds_access) {

  using namespace testing;
  // Instrument with single path component.
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};
  PathComponentInfo<MockFlatTree> pathComponentInfo(mockInstrumentTree);

  // All of these should be fine. Single index instrument.
  EXPECT_NO_THROW(pathComponentInfo.position(0));
  EXPECT_NO_THROW(pathComponentInfo.entryPoint(0));
  EXPECT_NO_THROW(pathComponentInfo.exitPoint(0));
  EXPECT_NO_THROW(pathComponentInfo.rotation(0));

  // All of these will be out of bounds

  EXPECT_THROW(pathComponentInfo.position(1), std::out_of_range);
  EXPECT_THROW(pathComponentInfo.entryPoint(1), std::out_of_range);
  EXPECT_THROW(pathComponentInfo.exitPoint(1), std::out_of_range);
  EXPECT_THROW(pathComponentInfo.rotation(1), std::out_of_range);

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}

void do_test_rotation(bool rotateAsSingleItem) {

  using namespace testing;
  // Instrument with single path component.
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nPathComponents())
      .WillRepeatedly(testing::Return(1));
  EXPECT_CALL(*pMockInstrumentTree, startPositions())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {1, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startRotations())
      .WillRepeatedly(testing::Return(std::vector<Eigen::Quaterniond>(
          1, Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {-1, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {2, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, pathComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>(1, 0)));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};
  PathComponentInfo<MockFlatTree> pathComponentInfo(mockInstrumentTree);

  size_t pathComponentIndex = 0;

  const Eigen::Vector3d rotationAxis{0, 0, 1}; // Rotate around z axis
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{0, 0, 0}; // rotate around origin

  if (rotateAsSingleItem) {
    pathComponentInfo.rotatePathComponent(pathComponentIndex, rotationAxis,
                                          rotationAngle, rotationCenter);
  } else {
    // Create a vector of the argument but otherwise do the same as above.
    std::vector<size_t> indexesToRotate(1, pathComponentIndex);
    pathComponentInfo.rotatePathComponents(indexesToRotate, rotationAxis,
                                           rotationAngle, rotationCenter);
  }

  auto after = pathComponentInfo.rotation(pathComponentIndex);
  auto rotMatrix = after.toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal rotation not updated correctly";

  EXPECT_TRUE(pathComponentInfo.position(pathComponentIndex)
                  .isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Position should be rotated around z";

  EXPECT_TRUE(pathComponentInfo.entryPoint(pathComponentIndex)
                  .isApprox(Eigen::Vector3d{0, -1, 0}, 1e-14))
      << "Entry point should be rotated around z";

  EXPECT_TRUE(pathComponentInfo.exitPoint(pathComponentIndex)
                  .isApprox(Eigen::Vector3d{0, 2, 0}, 1e-14))
      << "Exit point should be rotated around z";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(pMockInstrumentTree));
}

void do_test_position(bool moveAsSingleItem) {

  using namespace testing;
  // Instrument with single path component.
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nPathComponents())
      .WillRepeatedly(testing::Return(1));
  EXPECT_CALL(*pMockInstrumentTree, startPositions())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {1, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startRotations())
      .WillRepeatedly(testing::Return(std::vector<Eigen::Quaterniond>(
          1, Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {2, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints())
      .WillRepeatedly(
          testing::Return(std::vector<Eigen::Vector3d>(1, {3, 0, 0})));
  EXPECT_CALL(*pMockInstrumentTree, pathComponentIndexes())
      .WillRepeatedly(testing::Return(std::vector<size_t>(1, 0)));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};
  PathComponentInfo<MockFlatTree> pathComponentInfo(mockInstrumentTree);

  size_t pathComponentIndex = 0;
  Eigen::Vector3d offset{1, 0, 0};

  if (moveAsSingleItem) {
    pathComponentInfo.movePathComponent(pathComponentIndex, offset);
  } else {
    // Create a vector of the argument but otherwise do the same as above.
    std::vector<size_t> indexesToMove(1, pathComponentIndex);
    pathComponentInfo.movePathComponents(indexesToMove, offset);
  }

  EXPECT_TRUE(pathComponentInfo.position(pathComponentIndex)
                  .isApprox(Eigen::Vector3d{2, 0, 0}, 1e-14))
      << "Position should be shiftd along x";

  EXPECT_TRUE(pathComponentInfo.entryPoint(pathComponentIndex)
                  .isApprox(Eigen::Vector3d{3, 0, 0}, 1e-14))
      << "Entry should be shiftd along x";

  EXPECT_TRUE(pathComponentInfo.exitPoint(pathComponentIndex)
                  .isApprox(Eigen::Vector3d{4, 0, 0}, 1e-14))
      << "Exit should be shiftd along x";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(pMockInstrumentTree));
}

TEST(path_info_test, test_rotation) {

  bool rotateAsSingleItem = true;
  do_test_rotation(rotateAsSingleItem);
}

TEST(path_info_test, test_rotations) {

  bool rotateAsSingleItem = false;
  do_test_rotation(rotateAsSingleItem);
}

TEST(path_info_test, test_position) {
  bool moveAsSingleItem = true;
  do_test_position(moveAsSingleItem);
}

TEST(path_info_test, test_positions) {
  bool moveAsSingleItem = false;
  do_test_position(moveAsSingleItem);
}
}
