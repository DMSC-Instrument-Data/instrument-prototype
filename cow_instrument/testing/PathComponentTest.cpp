#include <gtest/gtest.h>
#include <string>
#include "PathComponent.h"
#include "Detector.h"
#include <Eigen/Core>

namespace {

/**
 * FakePathComponent class for testing PathComponent.
 */
class FakePathComponent : public PathComponent {
private:
  Eigen::Vector3d m_position;

  // Component interface
public:
  FakePathComponent(const Eigen::Vector3d &position) : m_position(position) {}
  Eigen::Vector3d getPos() const { return m_position; }
  virtual Eigen::Quaterniond getRotation() const override {
    return Eigen::Quaterniond::Identity();
  }
  void shiftPositionBy(const Eigen::Vector3d &){}
  void rotate(const Eigen::Vector3d&, const double&, const Eigen::Vector3d&){}
  void rotate(const Eigen::Affine3d &, const Eigen::Quaterniond &){};
  Component *clone() const { return new FakePathComponent{m_position}; }
  bool equals(const Component &other) const { return false; }
  void registerContents(LinkedTreeParser &) const {}
  void registerContents(LinkedTreeParser &, size_t) const {};
  ComponentIdType componentId() const { return ComponentIdType{1}; }
  std::string name() const { return ""; }
  bool accept(ComponentVisitor *) const { return false; }
};

TEST(path_component_test, test_default_length) {

  Eigen::Vector3d position{1, 1, 1};
  FakePathComponent pathComponent(position);
  EXPECT_EQ(pathComponent.length(), 0);
}

TEST(path_component_test, test_default_entryPoint) {

  Eigen::Vector3d position{1, 1, 1};
  FakePathComponent pathComponent(position);
  EXPECT_EQ(pathComponent.entryPoint(), position);
}

TEST(path_component_test, test_default_exitPoint) {

  Eigen::Vector3d position{1, 1, 1};
  FakePathComponent pathComponent(position);
  EXPECT_EQ(pathComponent.exitPoint(), position);
}
}
