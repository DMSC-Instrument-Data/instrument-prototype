#ifndef MOCKTYPES_H
#define MOCKTYPES_H

#include "Component.h"
#include "Detector.h"
#include "DetectorInfo.h"
#include "ComponentInfo.h"
#include "PathComponent.h"
#include "PathFactory.h"
#include "gmock/gmock.h"
#include <cow_ptr.h>
#include <vector>
#include <numeric>

class MockComponent : public Component {
public:
  MOCK_CONST_METHOD0(getPos, Eigen::Vector3d());
  MOCK_CONST_METHOD0(getRotation, Eigen::Quaterniond());
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD1(registerContents, void(LinkedTreeParser &));
  MOCK_CONST_METHOD2(registerContents, void(LinkedTreeParser &, size_t));

  ~MockComponent() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
  MOCK_CONST_METHOD1(accept, bool(ComponentVisitor *));
};

class MockPathComponent : public PathComponent {
public:
  MockPathComponent() {
    ON_CALL(*this, componentId())
        .WillByDefault(testing::Return(ComponentIdType(0)));
  }
  MOCK_CONST_METHOD0(getPos, Eigen::Vector3d());
  MOCK_CONST_METHOD0(getRotation, Eigen::Quaterniond());
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));

  void registerContents(LinkedTreeParser &info) const {
    info.registerPathComponent(this);
  }
  void registerContents(LinkedTreeParser &info, size_t parentIndex) const {
    info.registerPathComponent(this, parentIndex);
  }

  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
  MOCK_CONST_METHOD1(accept, bool(ComponentVisitor *));

  MOCK_CONST_METHOD0(length, double());
  MOCK_CONST_METHOD0(entryPoint, Eigen::Vector3d());
  MOCK_CONST_METHOD0(exitPoint, Eigen::Vector3d());
  MOCK_CONST_METHOD0(isSource, bool());
  MOCK_CONST_METHOD0(isSample, bool());
  ~MockPathComponent() {}
};

class MockDetector : public Detector {
public:
  MOCK_CONST_METHOD0(detectorId, DetectorIdType());
  MOCK_CONST_METHOD0(getPos, Eigen::Vector3d());
  MOCK_CONST_METHOD0(getRotation, Eigen::Quaterniond());
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD1(registerContents, void(LinkedTreeParser &));
  MOCK_CONST_METHOD2(registerContents, void(LinkedTreeParser &, size_t));
  ~MockDetector() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
  MOCK_CONST_METHOD1(accept, bool(ComponentVisitor *));
};

template <typename T> class PolymorphicFlatTree {
public:
  virtual size_t nDetectors() const = 0;
  virtual size_t nPathComponents() const = 0;
  virtual size_t samplePathIndex() const = 0;
  virtual size_t sourcePathIndex() const = 0;
  virtual size_t componentSize() const = 0;
  virtual std::vector<Eigen::Vector3d> startPositions() const = 0;
  virtual std::vector<Eigen::Quaterniond> startRotations() const = 0;
  virtual std::vector<size_t> subTreeIndexes(size_t proxyIndex) const = 0;
  virtual size_t detIndexToCompIndex(size_t detectorIndex) const = 0;
  virtual size_t pathIndexToCompIndex(size_t pathIndex) const = 0;
  virtual std::vector<Eigen::Vector3d> startEntryPoints() const = 0;
  virtual std::vector<Eigen::Vector3d> startExitPoints() const = 0;
  virtual std::vector<double> pathLengths() const = 0;
  virtual std::vector<size_t> detectorComponentIndexes() const = 0;
  virtual std::vector<size_t> pathComponentIndexes() const = 0;
  virtual ~PolymorphicFlatTree() {}
};

class MockFlatTree : public PolymorphicFlatTree<MockFlatTree> {
public:
  MockFlatTree() {
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(0));
    ON_CALL(*this, nPathComponents()).WillByDefault(testing::Return(1));
    ON_CALL(*this, detectorComponentIndexes())
        .WillByDefault(testing::Return(std::vector<size_t>(0, 0)));
    ON_CALL(*this, pathComponentIndexes())
        .WillByDefault(testing::Return(std::vector<size_t>(1, 0)));
    ON_CALL(*this, samplePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, sourcePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, componentSize()).WillByDefault(testing::Return(1));
    ON_CALL(*this, startPositions())
        .WillByDefault(testing::Return(
            std::vector<Eigen::Vector3d>(1 /*componentSize()*/, {0, 0, 0})));
    ON_CALL(*this, startRotations())
        .WillByDefault(testing::Return(std::vector<Eigen::Quaterniond>(
            1 /*componentSize()*/,
            Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
    ON_CALL(*this, startEntryPoints())
        .WillByDefault(testing::Return(
            std::vector<Eigen::Vector3d>(1 /*componentSize()*/, {0, 0, 0})));
    ON_CALL(*this, startExitPoints())
        .WillByDefault(testing::Return(
            std::vector<Eigen::Vector3d>(1 /*componentSize()*/, {0, 0, 0})));
    ON_CALL(*this, pathLengths())
        .WillByDefault(testing::Return(
            std::vector<double>(1 /*componentSize()*/, 0)));
  }

  MockFlatTree(size_t nDetectors) {
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(nDetectors));
    ON_CALL(*this, nPathComponents()).WillByDefault(testing::Return(1));
    ON_CALL(*this, detectorComponentIndexes())
        .WillByDefault(testing::Return(std::vector<size_t>(nDetectors, 0)));

    std::vector<size_t> pathComponentIndexesData(1);
    std::iota(pathComponentIndexesData.begin(), pathComponentIndexesData.end(),
              nDetectors);
    ON_CALL(*this, pathComponentIndexes())
        .WillByDefault(testing::Return(pathComponentIndexesData));

    ON_CALL(*this, samplePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, sourcePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, componentSize())
        .WillByDefault(testing::Return(nDetectors + 1));
    ON_CALL(*this, startPositions())
        .WillByDefault(testing::Return(
            std::vector<Eigen::Vector3d>(1 /*componentSize()*/, {0, 0, 0})));
    ON_CALL(*this, startRotations())
        .WillByDefault(testing::Return(std::vector<Eigen::Quaterniond>(
            1 /*componentSize()*/,
            Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
    ON_CALL(*this, startEntryPoints())
        .WillByDefault(testing::Return(
            std::vector<Eigen::Vector3d>(1 /*componentSize()*/, {0, 0, 0})));
    ON_CALL(*this, startExitPoints())
        .WillByDefault(testing::Return(
            std::vector<Eigen::Vector3d>(1 /*componentSize()*/, {0, 0, 0})));
    ON_CALL(*this, pathLengths())
        .WillByDefault(testing::Return(
            std::vector<double>(1 /*componentSize()*/, 0)));
  }
  MOCK_CONST_METHOD0(nDetectors, size_t());
  MOCK_CONST_METHOD0(nPathComponents, size_t());
  MOCK_CONST_METHOD1(getDetector, const Detector &(size_t));
  MOCK_CONST_METHOD0(samplePathIndex, size_t());
  MOCK_CONST_METHOD0(sourcePathIndex, size_t());
  MOCK_CONST_METHOD0(componentSize, size_t());
  MOCK_CONST_METHOD0(startPositions, std::vector<Eigen::Vector3d>());
  MOCK_CONST_METHOD0(startRotations, std::vector<Eigen::Quaterniond>());
  MOCK_CONST_METHOD1(subTreeIndexes, std::vector<size_t>(size_t));
  MOCK_CONST_METHOD1(detIndexToCompIndex, size_t(size_t));
  MOCK_CONST_METHOD1(pathIndexToCompIndex, size_t(size_t));
  MOCK_CONST_METHOD0(startEntryPoints, std::vector<Eigen::Vector3d>());
  MOCK_CONST_METHOD0(startExitPoints, std::vector<Eigen::Vector3d>());
  MOCK_CONST_METHOD0(pathLengths, std::vector<double>());
  MOCK_CONST_METHOD0(detectorComponentIndexes, std::vector<size_t>());
  MOCK_CONST_METHOD0(pathComponentIndexes, std::vector<size_t>());

  virtual ~MockFlatTree() {}
};

class MockPathFactory : public PathFactory<MockFlatTree> {

public:
  MOCK_CONST_METHOD1(createL2, Paths *(const MockFlatTree &instrument));
  MOCK_CONST_METHOD1(createL1, Paths *(const MockFlatTree &instrument));
  virtual ~MockPathFactory() {}
};

using NiceMockInstrumentTree = testing::NiceMock<MockFlatTree>;
using DetectorInfoWithMockInstrument = DetectorInfo<MockFlatTree>;
using DetectorInfoWithNiceMockInstrument = DetectorInfo<NiceMockInstrumentTree>;

using ComponentInfoWithMockInstrument = ComponentInfo<MockFlatTree>;
using ComponentInfoWithNiceMockInstrument =
    ComponentInfo<NiceMockInstrumentTree>;

#endif
