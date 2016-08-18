#include "gtest/gtest.h"
#include "PointPathComponent.h"

/*
 * Concrete tyep for testing purposes.
 */
class ToyPointPathComponent : public PointPathComponent<ToyPointPathComponent> {
public:
  using PointPathComponent<ToyPointPathComponent>::PointPathComponent;
  std::string getname() const { return "Toy point path component"; }
  virtual ~ToyPointPathComponent() {}
};

TEST(point_path_component_test, test_equals) {
  ToyPointPathComponent a(V3D{1, 1, 1}, ComponentIdType(1));
  ToyPointPathComponent b(V3D{1, 1, 1}, ComponentIdType(1));
  EXPECT_EQ(a, b);
  EXPECT_TRUE(a.equals(b));
}

TEST(point_path_component_test, test_not_equals) {
  ToyPointPathComponent reference(V3D{1, 1, 1}, ComponentIdType(1));

  ToyPointPathComponent differentId(V3D{1, 1, 1}, ComponentIdType(2));
  EXPECT_NE(reference, differentId);
  EXPECT_FALSE(reference.equals(differentId));

  ToyPointPathComponent differentPos(V3D{1, 1, 2}, ComponentIdType(1));
  EXPECT_NE(reference, differentPos);
  EXPECT_FALSE(reference.equals(differentPos));
}

TEST(point_path_component_test, clone) {
  ToyPointPathComponent original(V3D{1, 1, 1}, ComponentIdType(1));
  auto clone = original.clone();
  EXPECT_TRUE(clone->equals(original));
}

TEST(point_path_component_test, zero_length) {
  ToyPointPathComponent component(V3D{1, 1, 1}, ComponentIdType(1));
  EXPECT_EQ(component.length(), 0);
}

TEST(point_path_component_test, point_entry_exit) {
  ToyPointPathComponent component(V3D{1, 1, 1}, ComponentIdType(1));
  EXPECT_EQ(component.getPos(), component.exitPoint());
  EXPECT_EQ(component.getPos(), component.entryPoint());
}

TEST(point_path_component_test, test_rotate) {
  ToyPointPathComponent component(V3D{1, 0, 0}, ComponentIdType(1));

  const Eigen::Vector3d axis{0.0, 0.0, 1.0};
  const Eigen::Vector3d center{0.0, 0.0, 0.0};
  const double theta = M_PI / 2;

  // Rotate around z axis with centre at zero
  component.rotate(axis, theta, center);

  EXPECT_TRUE(component.getPos().isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14));

  Eigen::Vector3d rotatedVector =
      component.getRotation().toRotationMatrix() * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal PointPathComponent  rotation not updated correctly";
}

TEST(point_path_component_test, test_rotate_fast) {

  Eigen::Affine3d transform;
  Eigen::Quaterniond rotationPart;
  {
    using namespace Eigen;
    const Vector3d axis{0.0, 0.0, 1.0};
    const Vector3d center{0.0, 0.0, 0.0};
    const double theta = M_PI / 2;
    // Rotate it 90 degrees around z.
    transform = Translation3d(center) * AngleAxisd(theta, axis) *
                Translation3d(-center);
    rotationPart = transform.rotation();
  }

  ToyPointPathComponent component(V3D{1, 0, 0}, ComponentIdType(1));
  // Rotate it 90 degrees around z.
  component.rotate(transform, rotationPart);

  EXPECT_TRUE(component.getPos().isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14));
  Eigen::Vector3d rotatedVector =
      component.getRotation().toRotationMatrix() * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal PointPathComponent rotation not updated correctly";
}
