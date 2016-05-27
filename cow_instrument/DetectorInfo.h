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
#include "MaskFlags.h"
#include "MonitorFlags.h"
#include "L2s.h"
#include "Command.h"
#include <mutex>
#include <thread>

template <typename U>
void rangeCheck(size_t detectorIndex, const U &container) {
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
  template <typename V>
  DetectorInfo(V &&instrumentTree)
      : m_nDetectors(instrumentTree->nDetectors()),
        m_isMasked(m_nDetectors, Bool(false)),
        m_isMonitor(m_nDetectors, Bool(false)), m_l2(m_nDetectors),
        m_l2flags(m_nDetectors, Bool(false)),
        m_sourcePos(instrumentTree->sourcePos()),
        m_samplePos(instrumentTree->samplePos()),
        m_instrumentTree(instrumentTree) {

    /* The lifetime of the DetectorInfo is tied to the lifetime of the
       InstrumentTree (see constructor). So no danger that instrument movement
       gets
       out of step with this. InstrumentTrees are immutable.
     */

    // Calculate this once.
    m_l1 = distance(m_sourcePos, m_samplePos);
  }

  template <typename V>
  std::unique_ptr<DetectorInfo> cloneWithInstrumentTree(V &&instrumentTree) {

    if (instrumentTree.nDetectors() != m_instrumentTree.nDetectors()) {
      throw std::invalid_argument("The new InstrumentTree does not look the "
                                  "same as the existing InstrumentTree");
    }

    return std::unique_ptr<DetectorInfo>{
        new DetectorInfo(instrumentTree, *this)};
  }

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

    rangeCheck(detectorIndex, m_l2);
    if (!m_l2flags[detectorIndex]) {
      const Detector &det = m_instrumentTree->getDetector(detectorIndex);
      auto detPos = det.getPos();

      /*
       * Long-term is is not right. We cannot assume that the L2 path is simply
       * sample
       * to detector.
       */

      // TODO. These operations two following operations
      // need to be atomic and thread-safe per detectorIndex.
      m_l2[detectorIndex] = distance(detPos, m_samplePos);
      m_l2flags[detectorIndex] = true;
    }
    return m_l2[detectorIndex];
  }

  double l1() const { return m_l1; }
  double nDetectors() { return m_nDetectors; }

  const InstTree &const_instrumentTree() const { return *m_instrumentTree; }

  void modify(size_t nodeIndex, Command& command){
      m_instrumentTree = std::shared_ptr<InstTree>(m_instrumentTree->modify(command).release());
      // L2 cache is invalid.
      m_l2flags = L2Flags(m_nDetectors, Bool(false));
      // Meta-data should all still be valid.
  }

private:
  /**
   Private constructor, used by cloneWithInstrumentTree to
   replace the instrument tree, while keeping the rest of the metadata.
   */
  template <typename V>
  DetectorInfo(V &&instrumentTree, const DetectorInfo &metaDataSource)
      : m_nDetectors(instrumentTree->nDetectors()),
        m_isMasked(metaDataSource.m_isMasked),
        m_isMonitor(metaDataSource.m_isMonitor), m_l2(m_nDetectors),
        m_sourcePos(instrumentTree->sourcePos()),
        m_samplePos(instrumentTree->samplePos()),
        m_instrumentTree(instrumentTree) {

    m_l1 = distance(m_sourcePos, m_samplePos);
  }

  const std::shared_ptr<InstTree> m_instrumentTree;

  //------------------- MetaData -------------
  const size_t m_nDetectors;
  MaskFlags m_isMasked;     // This could be copied upon instrument change
  MonitorFlags m_isMonitor; // This could be copied upon instrument change

  //------------------- DerivedInfo
  double m_l1;               // This can't be copied upon instrument change
  const V3D m_sourcePos;     // This can't be copied upon instrument change
  const V3D m_samplePos;     // This can't be copied upon instrument change
  mutable L2s m_l2;          // This can't be copied upon instrument change
  mutable L2Flags m_l2flags; // This can't be copied upon instrument change
};

#endif
