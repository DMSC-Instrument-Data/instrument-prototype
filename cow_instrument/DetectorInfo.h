#ifndef DETECTOR_INFO_H
#define DETECTOR_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>
#include <boost/optional.hpp>
#include "Detector.h"
#include "IdType.h"

template <typename U>
void rangeCheck(size_t detectorIndex, const std::vector<U> &container) {
  if (detectorIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "Detector at index " << detectorIndex << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}

double distance(const V3D &a, const V3D &b) {
  return std::sqrt(((a[0] - b[0]) * (a[0] - b[0])) +
                   ((a[1] - b[1]) * (a[1] - b[1])) +
                   ((a[2] - b[2]) * (a[2] - b[2])));
}

template <typename InstTree> class DetectorInfo {
public:
  DetectorInfo(std::shared_ptr<InstTree> &&instrumentTree)
      : m_isMasked(instrumentTree->nDetectors(), false),
        m_isMonitor(instrumentTree->nDetectors(), false),
        m_l2(instrumentTree->nDetectors()),
        m_sourcePos(instrumentTree->sourcePos()),
        m_samplePos(instrumentTree->samplePos()),
        m_instrumentTree(instrumentTree) {}

  DetectorInfo(const std::shared_ptr<InstTree> &instrumentTree)
      : m_isMasked(instrumentTree->nDetectors(), false),
        m_isMonitor(instrumentTree->nDetectors(), false),
        m_l2(instrumentTree->nDetectors()),
        m_sourcePos(instrumentTree->sourcePos()),
        m_samplePos(instrumentTree->samplePos()),
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

  /* In some use cases scientists decide to use arbitrary
   * detectors or groups of detectors as monitors
   * so lets have the client code tell us which ones are monitors.
   */
  bool isMonitor(size_t detectorIndex) const {
    rangeCheck(detectorIndex, m_isMonitor);
    return m_isMonitor[detectorIndex];
  }

  double l2(size_t detectorIndex) const {
    /* The lifetime of the DetectorInfo is tied to the lifetime of the
       InstrumentTree (see constructor). So no danger that instrument movement
       gets
       out of step with this. InstrumentTrees are immutable.
     */
    rangeCheck(detectorIndex, m_l2);
    if (!m_l2[detectorIndex]) {
      const Detector &det = m_instrumentTree->getDetector(detectorIndex);
      auto detPos = det.getPos();

      m_l2[detectorIndex] = distance(detPos, m_samplePos);
    }
    return *m_l2[detectorIndex];
  }

  const InstTree &const_instrumentTree() const { return *m_instrumentTree; }

private:
  V3D m_sourcePos; // This can't be copied upon instrument change
  V3D m_samplePos; // This can't be copied upon instrument change
  std::shared_ptr<InstTree> m_instrumentTree;
  std::vector<bool> m_isMasked;  // This could be copied upon instrument change
  std::vector<bool> m_isMonitor; // This could be copied upon instrument change
  mutable std::vector<boost::optional<double>>
      m_l2; // This can't be copied upon instrument change
};

#endif
