#include "ScanTime.h"
#include <utility>
#include <limits>

ScanTime::ScanTime()
    : m_start(0), m_duration(std::numeric_limits<uint32_t>::max()) {}

ScanTime::ScanTime(const std::time_t &start, const size_t &duration)
    : m_start(start), m_duration(duration) {}

std::time_t ScanTime::end() const { return m_start + m_duration; }

std::time_t ScanTime::start() const { return m_start; }

size_t ScanTime::duration() const { return m_duration; }
