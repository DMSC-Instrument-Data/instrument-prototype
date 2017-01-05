#ifndef SCANTIME_H
#define SCANTIME_H

#include <ctime>
#include <cstdint>
#include <vector>

/**
 * This is a first attempt of defining a scan duration. This is completely
 * unoptimized.
 */
class ScanTime {
public:
  ScanTime();
  explicit ScanTime(const std::time_t &start, const size_t &duration);
  std::time_t end() const;
  std::time_t start() const;
  size_t duration() const;

private:
  /// Start time
  std::time_t m_start;
  /// End time
  std::time_t m_end;
  /// Duration in seconds.
  uint32_t m_duration;
};

using ScanTimes = std::vector<ScanTime>;

#endif
