#include "ComponentInfo.h"
#include "FlatTree.h"
#include "MockTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <stdexcept>
#include <memory>
#include "IdType.h"

namespace {

TEST(component_info_test, test_construct) {

  using namespace testing;
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(1));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};

  ComponentInfoWithMockInstrument{mockInstrumentTree};

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}

TEST(component_info_test, test_move) {

  using namespace testing;

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillOnce(Return(std::vector<size_t>{0}));

  ComponentInfoWithMockInstrument ComponentInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree)};

  auto before = ComponentInfo.position(0);
  auto offset = Eigen::Vector3d{1, 0, 0};
  ComponentInfo.move(0, offset);
  auto after = ComponentInfo.position(0);

  EXPECT_EQ(after, before + offset);
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(instrumentTree));
}

TEST(component_info_test, test_single_rotation_around_component_origin) {

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

  ComponentInfoWithMockInstrument ComponentInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree)};

  const size_t sampleComponentIndex = 0;

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{ComponentInfo.position(
      sampleComponentIndex)}; // rotate around component center

  ComponentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);

  auto after = ComponentInfo.rotation(sampleComponentIndex);
  auto rotMatrix = after.toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal rotation not updated correctly";

  EXPECT_TRUE(ComponentInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d{0, 0, 0}, 1e-14))
      << "Position should be unchanged as rotation was around its own center";

  EXPECT_TRUE(Mock::VerifyAndClearExpectations(instrumentTree));
}

TEST(component_info_test, test_multiple_rotation_around_component_origin) {

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

  ComponentInfoWithMockInstrument ComponentInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree)};

  const size_t sampleComponentIndex = 0;

  // Rotate sample once by 45 degrees
  ComponentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);
  // Rotate sample again by 45 degrees
  ComponentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);

  auto samplePosition = ComponentInfo.position(sampleComponentIndex);

  // Check that the position has not changed
  EXPECT_TRUE(samplePosition.isApprox(rotationCenter, 1e-14))
      << "Position should not change";

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix =
      ComponentInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(component_info_test, test_single_rotation_around_arbitrary_center) {

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

  ComponentInfoWithMockInstrument ComponentInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree)};
  const size_t sampleComponentIndex = 0;

  ComponentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);
  // Check that the position has the rotation applied.
  EXPECT_TRUE(ComponentInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d(0, 1, 0), 1e-14));

  // Check that the internal rotation gets updated
  Eigen::Matrix3d rotMatrix =
      ComponentInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(component_info_test, test_multiple_rotation_arbitrary_center) {

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

  ComponentInfoWithMockInstrument ComponentInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree)};
  const size_t sampleComponentIndex = 0;

  // Rotate once by 90 degrees around z should put detector at 0,1,0
  ComponentInfo.rotate(sampleComponentIndex, rotationAxisZ, rotationAngle,
                       rotationCenter);
  // Rotate again by 90 degrees around x should put detector at 0,0,1
  ComponentInfo.rotate(sampleComponentIndex, rotationAxisX, rotationAngle,
                       rotationCenter);

  // Check that the position has the rotations applied.
  EXPECT_TRUE(ComponentInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d(0, 0, 1), 1e-14));

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix =
      ComponentInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 0, 1}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(component_info_test, test_detector_info_constructor) {
  using namespace testing;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(0, Path{0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(0, Path{0})));

  auto detectorInfo = std::make_shared<DetectorInfo<NiceMockInstrumentTree>>(
      std::make_shared<NiceMockInstrumentTree>(), mockPathFactory);

  ComponentInfo<NiceMockInstrumentTree> compInfo{detectorInfo};
}
}
