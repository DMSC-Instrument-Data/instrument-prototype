#include <gtest/gtest.h>
#include <cmath>
#include "ParabolicGuide.h"

/*

                                                __,..--'""
                                       _,..--'""          |
                  s             _,..-'""                  |
                        _,..-'"                           |
                  _,.-'"                                  |
             _.-'"                                        |
         _.-"                                             | a
      .-'                                                 |
    .'                                                    |
   /                                                      |
  ;                            h                          |
  ;  -----------------------------------------------------|--
   \                                                      |
    `.                                                    |
      `-.                                                 |
         "-._                                             |
             "`-._                                        | a
                  "`-.,_                                  |
                        "`-..,_                           |
                               ""`-..,_                   |
                                       ""`--..,_          |
                                                ""`--..,__


 */

TEST(parabolic_guide_test, test_constructor_must_have_positive_a) {

  const double bad_a = -1;
  const double h = 1;
  const V3D pos = {0, 0, 0};
  EXPECT_THROW(ParabolicGuide(ComponentIdType(1), bad_a, h, pos),
               std::invalid_argument);
}

TEST(parabolic_guide_test, test_constructor_must_have_positive_h) {

  const double a = -1;
  const double bad_h = 1;
  const V3D pos = {0, 0, 0};
  EXPECT_THROW(ParabolicGuide(ComponentIdType(1), a, bad_h, pos),
               std::invalid_argument);
}

TEST(parabolic_guide_test, test_length_against_horizontal_straight_line) {

  /*
   We can make the Parabola approximate a stright line
   by mimimising h.
  */

  const double a = 2;
  const double h = 1E-9; // Close to zero.
  const V3D pos = {0, 0, 0};
  ParabolicGuide guide(ComponentIdType(1), a, h, pos);
  EXPECT_NEAR(guide.length(), 2.0, 1e-6) << "Parabolic length is wrong";
}

TEST(parabolic_guide_test, test_length_against_vertical_straight_line) {

  /*
   We can make the Parabola approximate a stright line
   by mimimising a.
  */

  const double a = 1E-9; // Close to zero.
  const double h = 2;
  const V3D pos = {0, 0, 0};
  ParabolicGuide guide(ComponentIdType(1), a, h, pos);
  EXPECT_NEAR(guide.length(), 2 * 2.0, 1e-6) << "Parabolic length is wrong";
}

TEST(parabolic_guide_test, test_length_against_simple_parabola) {

  const double a = 1.0;
  const double h = 1.0;
  const V3D pos = {0, 0, 0};
  ParabolicGuide guide(ComponentIdType(1), a, h, pos);

  const double expectedLength = std::sqrt(5.0) + (1.0 / 2.0) * std::asinh(2.0);

  EXPECT_NEAR(guide.length(), expectedLength, 1e-9)
      << "Parabolic length is wrong";
}

TEST(parabolic_guide_test, test_entry_exit_points) {

  const double a = 1.0;
  const V3D pos = {0, 0, 0};

  ParabolicGuide guide(
      ComponentIdType(1), a,
      1.0 /*h: This does not factor into the end or entry points*/, pos);

  EXPECT_EQ(guide.entryPoint(), V3D({pos[0] - a, pos[1], pos[2]}));
  EXPECT_EQ(guide.exitPoint(), V3D({pos[0] + a, pos[1], pos[2]}));
}

TEST(parabolic_guide_test, test_register_only_path_components) {

  ParabolicGuide guide(ComponentIdType(1), 1.0, 1.0, V3D{1.0, 1.0, 1.0});

  std::vector<const Detector *> detectorVec;
  std::vector<const PathComponent *> pathComponentVec;
  size_t detectorVecStartSize = detectorVec.size();
  size_t pathComponentVecSize = pathComponentVec.size();
  // Perform registration
  guide.registerContents(detectorVec, pathComponentVec);
  EXPECT_EQ(detectorVec.size(), detectorVecStartSize)
      << "Do not register ParabolicGuide as a Detector";
  EXPECT_EQ(pathComponentVec.size(), pathComponentVecSize + 1)
      << "ParabolicGuide should be registered as a PathComponent";
}

TEST(parabolic_guide_test, test_clone) {
  ParabolicGuide guide(ComponentIdType(1), 1.0, 1.0, V3D{1.0, 1.0, 1.0});
  ParabolicGuide *clone = guide.clone();

  EXPECT_TRUE(guide.equals(*clone));
}

TEST(parabolic_guide_test, test_equals) {

  const ComponentIdType id_1(1);
  const double a_1 = 1.0;
  const double h_1 = 1.0;
  const V3D v3d_1{1.0, 1.0, 1.0};

  ParabolicGuide guideOne(id_1, a_1, h_1, v3d_1);
  ParabolicGuide guideTwo(id_1, a_1, h_1, v3d_1);

  EXPECT_TRUE(guideOne.equals(guideTwo));
}

TEST(parabolic_guide_test, test_not_equals) {

  const ComponentIdType id_1(1);
  const double a_1 = 1.0;
  const double h_1 = 1.0;
  const V3D v3d_1{1.0, 1.0, 1.0};

  const ComponentIdType id_2(2);
  const double a_2 = 2.0;
  const double h_2 = 2.0;
  const V3D v3d_2{2.0, 1.0, 1.0};

  ParabolicGuide reference(id_1, a_1, h_1, v3d_1);

  // Try with different component id
  {
    ParabolicGuide differentComponentId(id_2, a_1, h_1, v3d_1);
    EXPECT_FALSE(reference.equals(differentComponentId));
  }

  // Try with different a
  {
    ParabolicGuide different_a(id_1, a_2, h_1, v3d_1);
    EXPECT_FALSE(reference.equals(different_a));
  }

  // Try with different h
  {
    ParabolicGuide different_h(id_1, a_1, h_2, v3d_1);
    EXPECT_FALSE(reference.equals(different_h));
  }

  // Try with different v3d
  {
    ParabolicGuide different_v3d(id_1, a_1, h_1, v3d_2);
    EXPECT_FALSE(reference.equals(different_v3d));
  }
}

TEST(parabolic_guide_test, test_rotate) {
  const ComponentIdType id(1);
  const double a = 1.0;
  const double h = 1.0;
  const Eigen::Vector3d position{1.0, 0.0, 0.0};
  const Eigen::Vector3d axis{0.0, 0.0, 1.0};
  const Eigen::Vector3d center{0.0, 0.0, 0.0};
  const double theta = M_PI / 2;

  ParabolicGuide guide(id, a, h, position);
  // Rotate it 90 degrees around z.
  guide.rotate(axis, theta, center);

  EXPECT_TRUE(guide.getPos().isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14));

  Eigen::Vector3d rotatedVector =
      guide.getRotation().toRotationMatrix() * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal guide rotation not updated correctly";
}

TEST(parabolic_guide_test, test_rotate_fast) {
  const ComponentIdType id(1);
  const double a = 1.0;
  const double h = 1.0;
  const Eigen::Vector3d position{1.0, 0.0, 0.0};

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

  ParabolicGuide guide(id, a, h, position);
  // Rotate it 90 degrees around z.
  guide.rotate(transform, rotationPart);

  EXPECT_TRUE(guide.getPos().isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14));
  Eigen::Vector3d rotatedVector =
      guide.getRotation().toRotationMatrix() * Eigen::Vector3d{1, 0, 0};
  EXPECT_TRUE(rotatedVector.isApprox(Eigen::Vector3d{0, 1, 0}, 1e-14))
      << "Internal guide rotation not updated correctly";
}
