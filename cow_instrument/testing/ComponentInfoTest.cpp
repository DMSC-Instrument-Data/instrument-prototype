#include "ComponentInfo.h"
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

TEST(component_info_test, test_construct) {

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

  ComponentInfoWithMockInstrument ComponentInfo(mockInstrumentTree,
                                                mockPathFactory);

  EXPECT_TRUE(testing::Mock::VerifyAndClear(pMockInstrumentTree))
      << "InstrumentTree used incorrectly";
  EXPECT_TRUE(testing::Mock::VerifyAndClear(&mockPathFactory))
      << "PathFactory used incorrectly";
}

TEST(component_info_test, test_construct_with_bad_l2_paths_throws) {

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
      ComponentInfoWithMockInstrument(mockInstrumentTree, mockPathFactory),
      std::logic_error);
}

TEST(component_info_test, test_construct_with_bad_l1_paths_throws) {

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
      ComponentInfoWithMockInstrument(mockInstrumentTree, mockPathFactory),
      std::logic_error);
}

TEST(component_info_test, test_get_l2s) {

  size_t nDetectors = 3;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0})));

  ComponentInfoWithNiceMockInstrument ComponentInfo{
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      mockPathFactory};

  auto l2s = ComponentInfo.l2s();

  EXPECT_EQ(l2s->size(), nDetectors);
}

TEST(component_info_test, test_calculate_l2_throw_out_of_range) {

  size_t nDetectors = 1;

  MockPathFactory mockPathFactory;
  EXPECT_CALL(mockPathFactory, createL1(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0, 0})));
  EXPECT_CALL(mockPathFactory, createL2(testing::_))
      .WillOnce(testing::Return(new Paths(nDetectors, Path{0})));

  ComponentInfoWithNiceMockInstrument ComponentInfo{
      std::make_shared<testing::NiceMock<MockFlatTree>>(nDetectors),
      mockPathFactory};

  EXPECT_THROW(ComponentInfo.l2(nDetectors), std::out_of_range);
}

TEST(component_info_test, test_calculate_l2) {

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

  ComponentInfoWithNiceMockInstrument ComponentInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};

  auto l2 = ComponentInfo.l2(0);
  EXPECT_EQ(l2, 20) << "sqrt((40 - 20)^2)";
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(pMockInstrumentTree));
}

TEST(component_info_test, test_calculate_l1) {
  using namespace testing;
  size_t nDetectors = 1;

  auto *pMockInstrumentTree = new testing::NiceMock<MockFlatTree>(nDetectors);

  /*
    Effectively a 2-component instrument.

    Source at x=0
    Sample at x = 20
  */
  EXPECT_CALL(*pMockInstrumentTree, pathLengths())
      .WillRepeatedly(Return(std::vector<double>(2, 0)));
  EXPECT_CALL(*pMockInstrumentTree, startEntryPoints())
      .WillRepeatedly(
          Return(std::vector<Eigen::Vector3d>{{0, 0, 3}, {0, 0, 5}}));
  EXPECT_CALL(*pMockInstrumentTree, startExitPoints())
      .WillRepeatedly(
          Return(std::vector<Eigen::Vector3d>{{0, 0, 3}, {0, 0, 5}}));
  EXPECT_CALL(*pMockInstrumentTree, sourcePathIndex())
      .WillRepeatedly(testing::Return(0));
  EXPECT_CALL(*pMockInstrumentTree, samplePathIndex())
      .WillRepeatedly(testing::Return(1));

  ComponentInfoWithNiceMockInstrument ComponentInfo{
      std::shared_ptr<NiceMockInstrumentTree>(pMockInstrumentTree),
      SourceSampleDetectorPathFactory<NiceMockInstrumentTree>{}};

  auto l1 = ComponentInfo.l1(0);
  EXPECT_EQ(l1, 2) << "sqrt((5 - 3)^2)";
  EXPECT_TRUE(testing::Mock::VerifyAndClearExpectations(pMockInstrumentTree));
}

TEST(component_info_test, test_move) {

  using namespace testing;

  auto *instrumentTree = new NiceMockInstrumentTree{};
  // configure what the subTreIndexes call will do. i.e. point to the first
  // component_id
  EXPECT_CALL(*instrumentTree, subTreeIndexes(_))
      .WillOnce(Return(std::vector<size_t>{0}));

  ComponentInfoWithMockInstrument ComponentInfo{
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

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
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

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
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};

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
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};
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
      std::shared_ptr<MockFlatTree>(instrumentTree),
      SourceSampleDetectorPathFactory<MockFlatTree>{}};
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
}
