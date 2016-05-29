#ifndef MOCKTYPES_H
#define MOCKTYPES_H

#include "Component.h"
#include "Command.h"
#include "Detector.h"
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

#endif
