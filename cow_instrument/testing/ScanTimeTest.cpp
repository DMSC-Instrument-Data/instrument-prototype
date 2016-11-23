#include "ScanTime.h"
#include <gtest/gtest.h>
#include <ctime>
#include <limits>

TEST(scan_time_test, test_alltime) {
  ScanTime scan;
  std::time_t epoch(0);
  auto expectedDuration = std::numeric_limits<uint32_t>::max();
  EXPECT_EQ(scan.start(), epoch);
  EXPECT_EQ(scan.end(), epoch + expectedDuration);
  EXPECT_EQ(scan.duration(), expectedDuration);
}

TEST(scan_time_test, test_real_time) {

  std::tm start;
  start.tm_hour = 1;
  size_t duration = 1; // second
  std::tm end = start;
  end.tm_sec = duration;

  ScanTime scan(std::mktime(&start), 1);

  EXPECT_EQ(scan.duration(), duration);
  EXPECT_EQ(scan.start(), std::mktime(&start));
  EXPECT_EQ(scan.end(), std::mktime(&end));
}
