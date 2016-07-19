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

class MockComponent : public Component {
public:
  MOCK_CONST_METHOD0(getPos, V3D());
  MOCK_METHOD1(deltaPos, void(const V3D &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD2(registerContents,
                     void(std::vector<const Detector *> &,
                          std::vector<const PathComponent *> &));
  ~MockComponent() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
};

class MockPathComponent : public PathComponent {
public:
  MOCK_CONST_METHOD0(getPos, V3D());
  MOCK_METHOD1(deltaPos, void(const V3D &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));

  void
  registerContents(std::vector<const Detector *> &,
                   std::vector<const PathComponent *> &pathComponents) const {
    pathComponents.push_back(this);
  }

  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());

  MOCK_CONST_METHOD0(length, double());
  MOCK_CONST_METHOD0(entryPoint, V3D());
  MOCK_CONST_METHOD0(exitPoint, V3D());
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

class MockDetector : public Detector {
public:
  MOCK_CONST_METHOD0(detectorId, DetectorIdType());
  MOCK_CONST_METHOD0(getPos, V3D());
  MOCK_METHOD1(deltaPos, void(const V3D &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD2(registerContents,
                     void(std::vector<const Detector *> &,
                          std::vector<const PathComponent *> &));
  ~MockDetector() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
};

template <typename T> class PolymorphicInstrumentTree {
public:
  virtual size_t nDetectors() const = 0;
  virtual const Detector &getDetector(size_t detectorIndex) const = 0;
  virtual const PathComponent &getPathComponent(size_t detectorIndex) const = 0;
  virtual std::unique_ptr<T> modify(size_t, const Command &command) const = 0;
  virtual size_t samplePathIndex() const = 0;
  virtual size_t sourcePathIndex() const = 0;
  virtual ~PolymorphicInstrumentTree() {}
};

class MockInstrumentTree
    : public PolymorphicInstrumentTree<MockInstrumentTree> {
public:
  MockInstrumentTree() {
    ON_CALL(m_detector, getPos()).WillByDefault(testing::Return(V3D{0, 0, 10}));
    ON_CALL(m_mockPathComponent, getPos())
        .WillByDefault(testing::Return(V3D{0, 0, 0}));
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(0));
    ON_CALL(*this, samplePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, sourcePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, getDetector(testing::_))
        .WillByDefault(testing::ReturnRef(m_detector));
    ON_CALL(*this, getPathComponent(testing::_))
        .WillByDefault(testing::ReturnRef(m_mockPathComponent));
  }

  MockInstrumentTree(size_t nDetectors) {
    ON_CALL(m_detector, getPos()).WillByDefault(testing::Return(V3D{0, 0, 10}));
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(nDetectors));
    ON_CALL(*this, samplePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, sourcePathIndex()).WillByDefault(testing::Return(size_t(0)));
    ON_CALL(*this, getDetector(testing::_))
        .WillByDefault(testing::ReturnRef(m_detector));
    ON_CALL(*this, getPathComponent(testing::_))
        .WillByDefault(testing::ReturnRef(m_mockPathComponent));
  }
  MOCK_CONST_METHOD0(nDetectors, size_t());
  MOCK_CONST_METHOD1(getDetector, const Detector &(size_t));
  MOCK_CONST_METHOD1(getPathComponent, const PathComponent &(size_t));
  MOCK_CONST_METHOD0(samplePathIndex, size_t());
  MOCK_CONST_METHOD0(sourcePathIndex, size_t());

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

class MockPathFactory : public PathFactory<InstrumentTree> {

public:
  MOCK_CONST_METHOD1(createL2, Paths(const InstrumentTree &instrument));
  virtual ~MockPathFactory() {}
};

using NiceMockInstrumentTree = testing::NiceMock<MockInstrumentTree>;
using DetectorInfoWithMockInstrument = DetectorInfo<MockInstrumentTree>;
using DetectorInfoWithNiceMockInstrument = DetectorInfo<NiceMockInstrumentTree>;

#endif
