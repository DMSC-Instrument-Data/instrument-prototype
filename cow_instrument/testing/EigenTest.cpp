#include <gtest/gtest.h>
#include "Eigen/Geometry"
#include <cmath>

TEST(eigen_test, rotate_point1) {

  Eigen::Vector3d a{1, 0, 0};
  double theta = M_PI / 2;
  Eigen::Quaterniond quat{Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitY())};

  auto b = quat.toRotationMatrix() * a;
  EXPECT_TRUE(b.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-12))
      << "Rotated 90 degrees around Y";
}

TEST(eigen_test, rotate_point2) {

  Eigen::Quaterniond a{0, 1, 0, 0}; // Point, x = 1, y = 0, z = 0
  double theta = M_PI / 2;

  Eigen::Quaterniond quat{Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitY())};

  // q.a.q^-1

  auto bq = quat * a * quat.inverse();
  EXPECT_TRUE(bq.vec().isApprox(Eigen::Vector3d{0, 0, -1}, 1e-12))
      << "Rotated 90 degrees around Y";
}

TEST(eigen_test, rotate_compound) {

  Eigen::Quaterniond a{0, 1, 0, 0}; // Point, x = 1, y = 0, z = 0
  double theta = M_PI / 2;
  Eigen::AngleAxisd aa(theta, Eigen::Vector3d::UnitY());
  Eigen::Quaterniond q{aa};
  Eigen::Quaterniond r{aa}; // same again

  // (q.r).a.(q.r)^-1

  auto bq = q * r * a * (q * r).inverse();
  EXPECT_TRUE(bq.vec().isApprox(Eigen::Vector3d{-1, 0, 0}, 1e-12))
      << "Rotated 90 degrees around Y. Twice";
}

TEST(eigen_test, rotate_relative_from_absolute_correct) {

  Eigen::Quaterniond a{0, 1, 0, 0}; // Point, x = 1, y = 0, z = 0
  double theta_base = M_PI / 2;
  Eigen::Quaterniond absolute_q_base{
      Eigen::AngleAxisd(theta_base, Eigen::Vector3d::UnitY())};
  double theta_desired = M_PI;
  Eigen::Quaterniond absolute_q_desired{
      Eigen::AngleAxisd(theta_desired, Eigen::Vector3d::UnitY())};
  Eigen::Quaterniond diff_q =
      absolute_q_desired * absolute_q_base.inverse(); // calculate the rotation

  // diff_q should be a rotation M_PI - M_PI/2 = M_PI/2

  auto bq = diff_q * a * diff_q.inverse(); // Apply the rotation to a point
  EXPECT_TRUE(bq.vec().isApprox(Eigen::Vector3d{0, 0, -1}, 1e-12))
      << "Diff is a rotation yielding 90 degrees around Y";
}

TEST(eigen_test, rotate_relative_from_absolute_correct2) {

  double theta = M_PI / 3;
  // Absolute rotations
  Eigen::Quaterniond grand_parent_q{
      Eigen::AngleAxisd(theta, Eigen::Vector3d::UnitY())}; // 30 degrees
  Eigen::Quaterniond parent_q{
      Eigen::AngleAxisd(theta * 2, Eigen::Vector3d::UnitY())}; // 60 degrees
  Eigen::Quaterniond current_q{
      Eigen::AngleAxisd(theta * 3, Eigen::Vector3d::UnitY())}; // 90 degrees

  Eigen::Quaterniond diff_1q =
      current_q * (grand_parent_q).inverse(); // PI - PI/3 = 2PI/3 = 120
  Eigen::Quaterniond diff_2q =
      current_q * (parent_q).inverse(); // PI - 2PI/3 = PI/3 = 60

  auto angle_between = diff_2q.angularDistance(diff_1q);

  EXPECT_NEAR(angle_between * (180 / M_PI), 60, 1e-12)
      << "We expect 120 - 60 = 60 degrees between these two diff rotations";
}
