#include "ComponentInfo.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "DetectorInfo.h"
#include "PointSample.h"
#include "PointSource.h"
#include "FlatTree.h"
#include "MockTypes.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <map>
#include <stdexcept>
#include <memory>
#include "IdType.h"

namespace {

std::shared_ptr<FlatTree> makeInstrumentTree() {
  /*

        A
        |
 ------------------------------
 |                |           |
 B                C           D
                              |
                              E


  */

  auto a = std::make_shared<CompositeComponent>(ComponentIdType(1));
  a->addComponent(std::unique_ptr<DetectorComponent>(new DetectorComponent(
      ComponentIdType(2), DetectorIdType(1), Eigen::Vector3d{1, 1, 1})));
  a->addComponent(std::unique_ptr<PointSource>(
      new PointSource(Eigen::Vector3d{-1, 0, 0}, ComponentIdType(3))));

  auto d = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(4)));
  d->addComponent(std::unique_ptr<PointSample>(
      new PointSample(Eigen::Vector3d{0.1, 0, 0}, ComponentIdType(5))));

  a->addComponent(std::move(d));

  return std::make_shared<FlatTree>(a);
}

TEST(component_info_test, test_construct) {

  using namespace testing;
  MockFlatTree *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>{};
  EXPECT_CALL(*pMockInstrumentTree, nDetectors())
      .WillRepeatedly(testing::Return(1));

  std::shared_ptr<MockFlatTree> mockInstrumentTree{pMockInstrumentTree};

  ComponentInfoWithMockInstrument{
      std::make_shared<DetectorInfo<MockFlatTree>>(mockInstrumentTree)};

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
}

TEST(component_info_test, test_move) {

  using namespace testing;

  auto *instrumentTree = new testing::NiceMock<MockFlatTree>{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillOnce(Return(std::vector<size_t>{0}));

  std::shared_ptr<NiceMockInstrumentTree> mockInstrumentTree(instrumentTree);
  auto componentInfo = ComponentInfoWithNiceMockInstrument{
      std::make_shared<DetectorInfo<NiceMockInstrumentTree>>(
          mockInstrumentTree)};

  auto before = componentInfo.position(0);
  auto offset = Eigen::Vector3d{1, 0, 0};
  componentInfo.move(0, offset);
  auto after = componentInfo.position(0);

  EXPECT_EQ(after, before + offset);
  EXPECT_TRUE(Mock::VerifyAndClearExpectations(instrumentTree));
}

TEST(component_info_test, test_single_rotation_around_component_origin) {

  using namespace testing;

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillRepeatedly(Return(std::vector<size_t>{0}));
  EXPECT_CALL(*instrumentTree, startPositions())
      .WillRepeatedly(
          Return(std::vector<Eigen::Vector3d>{Eigen::Vector3d{0, 0, 0}}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillRepeatedly(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  std::shared_ptr<NiceMockInstrumentTree> mockInstrumentTree(instrumentTree);
  auto componentInfo = ComponentInfoWithNiceMockInstrument{
      std::make_shared<DetectorInfo<NiceMockInstrumentTree>>(
          mockInstrumentTree)};

  const size_t sampleComponentIndex = 0;

  const Eigen::Vector3d rotationAxis{0, 0, 1};
  const double rotationAngle = M_PI / 2;
  const Eigen::Vector3d rotationCenter{componentInfo.position(
      sampleComponentIndex)}; // rotate around component center

  componentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);

  auto after = componentInfo.rotation(sampleComponentIndex);
  auto rotMatrix = after.toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal rotation not updated correctly";

  EXPECT_TRUE(componentInfo.position(sampleComponentIndex)
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
      .WillRepeatedly(Return(std::vector<Eigen::Vector3d>{rotationCenter}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillRepeatedly(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  std::shared_ptr<NiceMockInstrumentTree> mockInstrumentTree(instrumentTree);
  auto componentInfo = ComponentInfoWithNiceMockInstrument{
      std::make_shared<DetectorInfo<NiceMockInstrumentTree>>(
          mockInstrumentTree)};

  const size_t sampleComponentIndex = 0;

  // Rotate sample once by 45 degrees
  componentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);
  // Rotate sample again by 45 degrees
  componentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);

  auto samplePosition = componentInfo.position(sampleComponentIndex);

  // Check that the position has not changed
  EXPECT_TRUE(samplePosition.isApprox(rotationCenter, 1e-14))
      << "Position should not change";

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix =
      componentInfo.rotation(sampleComponentIndex).toRotationMatrix();
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
      .WillRepeatedly(Return(std::vector<Eigen::Vector3d>{componentCenter}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillRepeatedly(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  std::shared_ptr<NiceMockInstrumentTree> mockInstrumentTree(instrumentTree);
  auto componentInfo = ComponentInfoWithNiceMockInstrument{
      std::make_shared<DetectorInfo<NiceMockInstrumentTree>>(
          mockInstrumentTree)};
  const size_t sampleComponentIndex = 0;

  componentInfo.rotate(sampleComponentIndex, rotationAxis, rotationAngle,
                       rotationCenter);
  // Check that the position has the rotation applied.
  EXPECT_TRUE(componentInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d(0, 1, 0), 1e-14));

  // Check that the internal rotation gets updated
  Eigen::Matrix3d rotMatrix =
      componentInfo.rotation(sampleComponentIndex).toRotationMatrix();
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
      .WillRepeatedly(Return(std::vector<Eigen::Vector3d>{componentCenter}));
  EXPECT_CALL(*instrumentTree, startRotations())
      .WillRepeatedly(Return(std::vector<Eigen::Quaterniond>{
          Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()}}));

  std::shared_ptr<NiceMockInstrumentTree> mockInstrumentTree(instrumentTree);
  auto componentInfo = ComponentInfoWithNiceMockInstrument{
      std::make_shared<DetectorInfo<NiceMockInstrumentTree>>(
          mockInstrumentTree)};
  const size_t sampleComponentIndex = 0;

  // Rotate once by 90 degrees around z should put detector at 0,1,0
  componentInfo.rotate(sampleComponentIndex, rotationAxisZ, rotationAngle,
                       rotationCenter);
  // Rotate again by 90 degrees around x should put detector at 0,0,1
  componentInfo.rotate(sampleComponentIndex, rotationAxisX, rotationAngle,
                       rotationCenter);

  // Check that the position has the rotations applied.
  EXPECT_TRUE(componentInfo.position(sampleComponentIndex)
                  .isApprox(Eigen::Vector3d(0, 0, 1), 1e-14));

  // Check that the internal rotation gets updated. i.e component is rotated
  // around its own centre.
  Eigen::Matrix3d rotMatrix =
      componentInfo.rotation(sampleComponentIndex).toRotationMatrix();
  // Check that some vector I define gets rotated as I would expect
  Eigen::Vector3d rotatedVector = rotMatrix * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 0, 1}, 1e-14))
      << "Internal component rotation not updated correctly";
}

TEST(component_info_test, test_position) {

  auto detectorInfo =
      std::make_shared<DetectorInfo<FlatTree>>(makeInstrumentTree());

  ComponentInfo<FlatTree> componentInfo(detectorInfo);

  auto posB = componentInfo.position(1); // Detector
  auto posC = componentInfo.position(2); // path component (source)
  auto posD = componentInfo.position(3); // composite
  auto posE = componentInfo.position(4); // path component (sample)

  EXPECT_EQ(posB, Eigen::Vector3d(1, 1, 1)) << "Detector position incorrect";
  EXPECT_EQ(posC, Eigen::Vector3d(-1, 0, 0)) << "Source position incorrect";
  EXPECT_EQ(posE, Eigen::Vector3d(0.1, 0, 0)) << "Sanity check position of E";
  EXPECT_EQ(posD, posE) << "D is just a composite containing E, so the "
                           "positions should be the same";

  auto posA = componentInfo.position(0); // composite (whole instrument)

  EXPECT_EQ(posA, (posB + posC + posE) / 3);
  EXPECT_NE(posA, (posB + posC + posE + posD) / 4)
      << "Composites (posD) should not be factored in";
}
}
