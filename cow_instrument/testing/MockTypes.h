#ifndef MOCKTYPES_H
#define MOCKTYPES_H

#include "Component.h"
#include "Command.h"
#include "Detector.h"
#include "DetectorInfo.h"
#include "gmock/gmock.h"

class MockComponent : public Component {
public:
  MOCK_CONST_METHOD0(getPos, V3D());
  MOCK_METHOD1(deltaPos, void(const V3D &));
  MOCK_CONST_METHOD0(clone, Component *());
  MOCK_CONST_METHOD1(equals, bool(const Component &));
  MOCK_CONST_METHOD1(registerContents, void(std::vector<const Detector *> &));
  ~MockComponent() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
};

class MockCommand : public Command {
public:
  MockCommand(){
      using namespace testing;
      ON_CALL(*this, isMetaDataCommand()).WillByDefault(Return(false));
  }
  MOCK_CONST_METHOD1(execute, void(Component &));
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
  MOCK_CONST_METHOD1(registerContents, void(std::vector<const Detector *> &));
  ~MockDetector() {}
  MOCK_CONST_METHOD0(componentId, ComponentIdType());
  MOCK_CONST_METHOD0(name, std::string());
};

template <typename T> class PolymorphicInstrumentTree {
public:
  virtual size_t nDetectors() const = 0;
  virtual V3D sourcePos() const = 0;
  virtual V3D samplePos() const = 0;
  virtual const Detector &getDetector(size_t detectorIndex) const = 0;
  virtual std::unique_ptr<T> modify(const Command &command) const = 0;
  virtual ~PolymorphicInstrumentTree() {}
};

class MockInstrumentTree
    : public PolymorphicInstrumentTree<MockInstrumentTree> {
public:
  MockInstrumentTree() {
    ON_CALL(m_detector, getPos()).WillByDefault(testing::Return(V3D{0,0,10}));
    ON_CALL(*this, samplePos()).WillByDefault(testing::Return(V3D{0, 0, 20}));
    ON_CALL(*this, sourcePos()).WillByDefault(testing::Return(V3D{0, 0, 0}));
    ON_CALL(*this, getDetector(testing::_)).WillByDefault(testing::ReturnRef(m_detector));
  }
  MockInstrumentTree(size_t nDetectors) {
    ON_CALL(m_detector, getPos()).WillByDefault(testing::Return(V3D{0,0,10}));
    ON_CALL(*this, nDetectors()).WillByDefault(testing::Return(nDetectors));
    ON_CALL(*this, samplePos()).WillByDefault(testing::Return(V3D{0, 0, 20}));
    ON_CALL(*this, sourcePos()).WillByDefault(testing::Return(V3D{0, 0, 0}));
    ON_CALL(*this, getDetector(testing::_)).WillByDefault(testing::ReturnRef(m_detector));
  }
  MOCK_CONST_METHOD0(nDetectors, size_t());
  MOCK_CONST_METHOD0(sourcePos, V3D());
  MOCK_CONST_METHOD0(samplePos, V3D());
  MOCK_CONST_METHOD1(getDetector, const Detector &(size_t));

  std::unique_ptr<MockInstrumentTree> modify(const Command &command) const {
    return std::unique_ptr<MockInstrumentTree>(modifyProxy(command));
  }

  MOCK_CONST_METHOD1(modifyProxy, MockInstrumentTree *(const Command &));

  virtual ~MockInstrumentTree() {}
private:
  testing::NiceMock<MockDetector> m_detector;
};

using NiceMockInstrumentTree = testing::NiceMock<MockInstrumentTree>;
using DetectorInfoWithMockInstrument = DetectorInfo<MockInstrumentTree>;
using DetectorInfoWithNiceMockInstrument = DetectorInfo<NiceMockInstrumentTree>;

#endif
