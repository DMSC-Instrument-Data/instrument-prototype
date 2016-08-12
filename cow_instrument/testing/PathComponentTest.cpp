#include <gtest/gtest.h>
#include <string>
#include "PathComponent.h"
#include "Detector.h"
#include "V3D.h"

namespace {

/**
 * FakePathComponent class for testing PathComponent.
 */
class FakePathComponent : public PathComponent {
private:
  V3D m_position;

  // Component interface
public:
  FakePathComponent(const V3D &position) : m_position(position) {}
  V3D getPos() const { return m_position; }
  void shiftPositionBy(const V3D &){};
  Component *clone() const { return new FakePathComponent{m_position}; };
  bool equals(const Component &other) const { return false; };
  void registerContents(std::vector<const Detector *> &,
                        std::vector<const PathComponent *> &) const {}
  ComponentIdType componentId() const { return ComponentIdType{1}; }
  std::string name() const { return ""; }
  void accept(ComponentVisitor *) const {}
};

TEST(path_component_test, test_default_length) {

  V3D position{1, 1, 1};
  FakePathComponent pathComponent(position);
  EXPECT_EQ(pathComponent.length(), 0);
}

TEST(path_component_test, test_default_entryPoint) {

  V3D position{1, 1, 1};
  FakePathComponent pathComponent(position);
  EXPECT_EQ(pathComponent.entryPoint(), position);
}

TEST(path_component_test, test_default_exitPoint) {

  V3D position{1, 1, 1};
  FakePathComponent pathComponent(position);
  EXPECT_EQ(pathComponent.exitPoint(), position);
}
}
