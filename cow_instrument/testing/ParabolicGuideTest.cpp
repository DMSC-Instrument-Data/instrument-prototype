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
  const Eigen::Vector3d pos = {0, 0, 0};
  EXPECT_THROW(ParabolicGuide(ComponentIdType(1), bad_a, h, pos),
               std::invalid_argument);
}

TEST(parabolic_guide_test, test_constructor_must_have_positive_h) {

  const double a = -1;
  const double bad_h = 1;
  const Eigen::Vector3d pos = {0, 0, 0};
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
  const Eigen::Vector3d pos = {0, 0, 0};
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
  const Eigen::Vector3d pos = {0, 0, 0};
  ParabolicGuide guide(ComponentIdType(1), a, h, pos);
  EXPECT_NEAR(guide.length(), 2 * 2.0, 1e-6) << "Parabolic length is wrong";
}

TEST(parabolic_guide_test, test_length_against_simple_parabola) {

  const double a = 1.0;
  const double h = 1.0;
  const Eigen::Vector3d pos = {0, 0, 0};
  ParabolicGuide guide(ComponentIdType(1), a, h, pos);

  const double expectedLength = std::sqrt(5.0) + (1.0 / 2.0) * std::asinh(2.0);

  EXPECT_NEAR(guide.length(), expectedLength, 1e-9)
      << "Parabolic length is wrong";
}

TEST(parabolic_guide_test, test_entry_exit_points) {

  const double a = 1.0;
  const Eigen::Vector3d pos = {0, 0, 0};

  ParabolicGuide guide(
      ComponentIdType(1), a,
      1.0 /*h: This does not factor into the end or entry points*/, pos);

  EXPECT_EQ(guide.entryPoint(), Eigen::Vector3d({pos[0] - a, pos[1], pos[2]}));
  EXPECT_EQ(guide.exitPoint(), Eigen::Vector3d({pos[0] + a, pos[1], pos[2]}));
}

TEST(parabolic_guide_test, test_register_only_path_components) {

  ParabolicGuide guide(ComponentIdType(1), 1.0, 1.0, Eigen::Vector3d{1.0, 1.0, 1.0});

  LinkedTreeParser info;
  // Perform registration
  guide.registerContents(info);
  EXPECT_EQ(info.detectorSize(), 0)
      << "Do not register ParabolicGuide as a Detector";
  EXPECT_EQ(info.pathSize(), 1)
      << "ParabolicGuide should be registered as a PathComponent";
}

TEST(parabolic_guide_test, test_clone) {
  ParabolicGuide guide(ComponentIdType(1), 1.0, 1.0, Eigen::Vector3d{1.0, 1.0, 1.0});
  ParabolicGuide *clone = guide.clone();

  EXPECT_TRUE(guide.equals(*clone));
}

TEST(parabolic_guide_test, test_equals) {

  const ComponentIdType id_1(1);
  const double a_1 = 1.0;
  const double h_1 = 1.0;
  const Eigen::Vector3d v3d_1{1.0, 1.0, 1.0};

  ParabolicGuide guideOne(id_1, a_1, h_1, v3d_1);
  ParabolicGuide guideTwo(id_1, a_1, h_1, v3d_1);

  EXPECT_TRUE(guideOne.equals(guideTwo));
}

TEST(parabolic_guide_test, test_not_equals) {

  const ComponentIdType id_1(1);
  const double a_1 = 1.0;
  const double h_1 = 1.0;
  const Eigen::Vector3d v3d_1{1.0, 1.0, 1.0};

  const ComponentIdType id_2(2);
  const double a_2 = 2.0;
  const double h_2 = 2.0;
  const Eigen::Vector3d v3d_2{2.0, 1.0, 1.0};

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

TEST(parabolic_guide_test, test_register_contents) {

  ParabolicGuide guide(ComponentIdType(1), 2, 1E-9, {0, 0, 0});

  // Registers
  LinkedTreeParser info;

  guide.registerContents(info);

  EXPECT_EQ(info.detectorSize(), 0);
  EXPECT_EQ(info.pathSize(), 1);
  EXPECT_EQ(info.pathComponentIndexes().size(), 1);
  EXPECT_EQ(info.detectorComponentIndexes().size(), 0);
  EXPECT_EQ(info.proxies().size(), 1) << "Proxies should grow";

  EXPECT_FALSE(info.proxies()[0].hasParent());
  EXPECT_FALSE(info.proxies()[0].hasChildren());
  EXPECT_EQ(info.pathComponentIndexes()[0], 0)
      << "Should be pointing to the zeroth index of proxies";
}
