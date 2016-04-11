#ifndef DETECTOR_INFO_H
#define DETECTOR_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include "IdType.h"

template <typename U>
void rangeCheck(size_t detectorIndex, const std::vector<U> &container) {
  if (detectorIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "Detector at index " << detectorIndex << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}

template <typename InstTree> class DetectorInfo {
public:
  DetectorInfo(std::shared_ptr<InstTree> &&instrumentTree)
      : m_isMasked(instrumentTree->nDetectors(), false),
        m_isMonitor(instrumentTree->nDetectors(), false),
        m_instrumentTree(instrumentTree) {}

  DetectorInfo(std::shared_ptr<InstTree> &instrumentTree)
      : m_isMasked(instrumentTree->nDetectors(), false),
        m_isMonitor(instrumentTree->nDetectors(), false),
        m_instrumentTree(instrumentTree) {}

  void setMasked(size_t detectorIndex) {
    rangeCheck(detectorIndex, m_isMasked);
    m_isMasked[detectorIndex] = true;
  }
  bool isMasked(size_t detectorIndex) const {
    rangeCheck(detectorIndex, m_isMasked);
    return m_isMasked[detectorIndex];
  }

  void setMonitor(size_t detectorIndex) {
    rangeCheck(detectorIndex, m_isMonitor);
    m_isMonitor[detectorIndex] = true;
  }

  bool isMonitor(size_t detectorIndex) const {
    rangeCheck(detectorIndex, m_isMonitor);
    return m_isMonitor[detectorIndex];
  }

private:
  std::shared_ptr<InstTree> m_instrumentTree;
  std::vector<bool> m_isMasked;
  std::vector<bool> m_isMonitor;
};

#endif
