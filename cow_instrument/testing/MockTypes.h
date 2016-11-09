#ifndef MOCKTYPES_H
#define MOCKTYPES_H

#include "Component.h"
#include "Command.h"
#include "Detector.h"
#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "PathComponent.h"
#include "PathFactory.h"
#include "gmock/gmock.h"
#include <cow_ptr.h>
#include <vector>

class MockComponent : public Component {
public:
  MOCK_CONST_METHOD0(getPos, Eigen::Vector3d());
  MOCK_CONST_METHOD0(getRotation, Eigen::Quaterniond());
  MOCK_METHOD1(shiftPositionBy, void(const Eigen::Vector3d &));
  MOCK_METHOD3(rotate, void(const Eigen::Vector3d&, const double&, const Eigen::Vector3d&));
  MOCK_METHOD2(rotate,
               void(const Eigen::Affine3d &, const Eigen::Quaterniond &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD1(registerContents, void(ComponentInfo &));
  MOCK_CONST_METHOD2(registerContents, void(ComponentInfo &, size_t));

  ~MockComponent() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
  MOCK_CONST_METHOD1(accept, bool(ComponentVisitor *));
};

class MockPathComponent : public PathComponent {
public:
  MOCK_CONST_METHOD0(getPos, Eigen::Vector3d());
  MOCK_CONST_METHOD0(getRotation, Eigen::Quaterniond());
  MOCK_METHOD1(shiftPositionBy, void(const Eigen::Vector3d &));
  MOCK_METHOD3(rotate, void(const Eigen::Vector3d&, const double&, const Eigen::Vector3d&));
  MOCK_METHOD2(rotate,
               void(const Eigen::Affine3d &, const Eigen::Quaterniond &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));

  void registerContents(ComponentInfo &info) const {
    info.registerPathComponent(this);
  }
  void registerContents(ComponentInfo &info, size_t parentIndex) const {
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

class MockCommand : public Command {
public:
  MockCommand() {
    using namespace testing;
    ON_CALL(*this, isMetaDataCommand()).WillByDefault(Return(false));
  }
  MOCK_CONST_METHOD1(execute, bool(CowPtr<Component> &));
  MOCK_CONST_METHOD0(isMetaDataCommand, bool());
  ~MockCommand() {}
};

class MockCommand2 : public Command2 {
public:
  MockCommand2() {
    using namespace testing;
    ON_CALL(*this, isMetaDataCommand()).WillByDefault(Return(false));
  }
  MOCK_CONST_METHOD1(execute, void(size_t));
  MOCK_CONST_METHOD0(isMetaDataCommand, bool());
  ~MockCommand2() {}
};

class MockDetector : public Detector {
public:
  MOCK_CONST_METHOD0(detectorId, DetectorIdType());
  MOCK_CONST_METHOD0(getPos, Eigen::Vector3d());
  MOCK_CONST_METHOD0(getRotation, Eigen::Quaterniond());
  MOCK_METHOD1(shiftPositionBy, void(const Eigen::Vector3d &));
  MOCK_METHOD3(rotate, void(const Eigen::Vector3d&, const double&, const Eigen::Vector3d&));
  MOCK_METHOD2(rotate,
               void(const Eigen::Affine3d &, const Eigen::Quaterniond &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD1(registerContents, void(ComponentInfo &));
  MOCK_CONST_METHOD2(registerContents, void(ComponentInfo &, size_t));
  ~MockDetector() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
  MOCK_CONST_METHOD1(accept, bool(ComponentVisitor *));
};

template <typename T> class PolymorphicInstrumentTree {
public:
  virtual size_t nDetectors() const = 0;
  virtual const Detector &getDetector(size_t detectorIndex) const = 0;
  virtual const PathComponent &getPathComponent(size_t detectorIndex) const = 0;
  virtual std::unique_ptr<T> modify(size_t, const Command &command) const = 0;
  virtual size_t samplePathIndex() const = 0;
  virtual size_t sourcePathIndex() const = 0;
  virtual size_t componentSize() const = 0;
  virtual std::vector<Eigen::Vector3d> startPositions() const = 0;
  virtual std::vector<Eigen::Quaterniond> startRotations() const = 0;
  virtual ~PolymorphicInstrumentTree() {}
};

class MockInstrumentTree
    : public PolymorphicInstrumentTree<MockInstrumentTree> {
public:
  MockInstrumentTree() {
    ON_CALL(m_detector, getPos()).WillByDefault(testing::Return(Eigen::Vector3d{0, 0, 10}));
    ON_CALL(m_mockPathComponent, getPos())
        .WillByDefault(testing::Return(Eigen::Vector3d{0, 0, 0}));
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(0));
    ON_CALL(*this, samplePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, sourcePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, getDetector(testing::_))
        .WillByDefault(testing::ReturnRef(m_detector));
    ON_CALL(*this, getPathComponent(testing::_))
        .WillByDefault(testing::ReturnRef(m_mockPathComponent));
    ON_CALL(*this, componentSize()).WillByDefault(testing::Return(1));
    ON_CALL(*this, startPositions())
        .WillByDefault(
            testing::Return(std::vector<Eigen::Vector3d>(1, {0, 0, 0})));
    ON_CALL(*this, startRotations())
        .WillByDefault(testing::Return(std::vector<Eigen::Quaterniond>(
            1, Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  }

  MockInstrumentTree(size_t nDetectors) {
    ON_CALL(m_detector, getPos()).WillByDefault(testing::Return(Eigen::Vector3d{0, 0, 10}));
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(nDetectors));
    ON_CALL(*this, samplePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, sourcePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, getDetector(testing::_))
        .WillByDefault(testing::ReturnRef(m_detector));
    ON_CALL(*this, getPathComponent(testing::_))
        .WillByDefault(testing::ReturnRef(m_mockPathComponent));
    ON_CALL(*this, componentSize())
        .WillByDefault(testing::Return(nDetectors + 1));
    ON_CALL(*this, startPositions())
        .WillByDefault(
            testing::Return(std::vector<Eigen::Vector3d>(1, {0, 0, 0})));
    ON_CALL(*this, startRotations())
        .WillByDefault(testing::Return(std::vector<Eigen::Quaterniond>(
            1, Eigen::Quaterniond(Eigen::Affine3d::Identity().rotation()))));
  }
  MOCK_CONST_METHOD0(nDetectors, size_t());
  MOCK_CONST_METHOD1(getDetector, const Detector &(size_t));
  MOCK_CONST_METHOD1(getPathComponent, const PathComponent &(size_t));
  MOCK_CONST_METHOD0(samplePathIndex, size_t());
  MOCK_CONST_METHOD0(sourcePathIndex, size_t());
  MOCK_CONST_METHOD0(componentSize, size_t());
  MOCK_CONST_METHOD0(startPositions, std::vector<Eigen::Vector3d>());
  MOCK_CONST_METHOD0(startRotations, std::vector<Eigen::Quaterniond>());

  std::unique_ptr<MockInstrumentTree> modify(size_t nodeIndex,
                                             const Command &command) const {
    return std::unique_ptr<MockInstrumentTree>(modifyProxy(nodeIndex, command));
  }

  MOCK_CONST_METHOD2(modifyProxy,
                     MockInstrumentTree *(size_t, const Command &));

  virtual ~MockInstrumentTree() {}

private:
  testing::NiceMock<MockDetector> m_detector;
  testing::NiceMock<MockPathComponent> m_mockPathComponent;
};

class MockPathFactory : public PathFactory<MockInstrumentTree> {

public:
  MOCK_CONST_METHOD1(createL2, Paths *(const MockInstrumentTree &instrument));
  MOCK_CONST_METHOD1(createL1, Paths *(const MockInstrumentTree &instrument));
  virtual ~MockPathFactory() {}
};

using NiceMockInstrumentTree = testing::NiceMock<MockInstrumentTree>;
using DetectorInfoWithMockInstrument = DetectorInfo<MockInstrumentTree>;
using DetectorInfoWithNiceMockInstrument = DetectorInfo<NiceMockInstrumentTree>;

#endif
