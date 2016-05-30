#include "gtest/gtest.h"
#include "Spectrum.h"

TEST(spectrum_test, test_move_construct_from_vector) {

  size_t sizeToMake = 1;
  size_t valueToHave = 1;
  std::vector<size_t> vec(sizeToMake, valueToHave);
  Spectrum a(std::move(vec));

  EXPECT_EQ(a.size(), sizeToMake);
  EXPECT_EQ(a[0], valueToHave);
}

TEST(spectrum_test, test_construct_from_vector) {

  std::vector<size_t> vec(1, 2);
  Spectrum a(vec);

  EXPECT_EQ(a.size(), vec.size());
  EXPECT_EQ(a[0], vec[0]);
}

TEST(spectrum_test, test_construct_from_list) {
  Spectrum a{1, 2, 3};

  EXPECT_EQ(a.size(), 3);
  EXPECT_EQ(a[0], 1);
  EXPECT_EQ(a[1], 2);
  EXPECT_EQ(a[2], 3);
}

TEST(spectrum_test, test_copy_construction) {
  Spectrum a{1, 2, 3};
  Spectrum b = a;
  EXPECT_EQ(a, b);
}

TEST(spectrum_test, test_assignment) {
  Spectrum a{1, 2, 4};
  Spectrum b{1};
  EXPECT_NE(a, b);
  b = a;
  EXPECT_EQ(b, a);
}

TEST(spectrum_test, test_move_assignment) {
  Spectrum a{1, 2, 4};
  Spectrum b{1};
  EXPECT_NE(a, b);
  b = std::move(a);
  EXPECT_EQ(b.size(), 3);
}

TEST(spectrum_test, test_assign_at_index) {
  Spectrum spectrum{1, 2, 3};
  spectrum[2] = 4;
  Spectrum expected{1, 2, 4};
  EXPECT_EQ(spectrum, expected);
}

TEST(spectrum_test, allow_duplicate_detector_ids) {
  // Im adding this to characterise the existing behaviour.
  Spectrum invalidSpectrum{1, 1};
}
