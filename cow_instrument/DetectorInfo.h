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
#include "Spectrum.h"
#include "cow_ptr.h"
#include <mutex>
#include <thread>

namespace {

template <typename U>
void detectorRangeCheck(size_t detectorIndex, const U &container) {
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
}

template <typename InstTree> class DetectorInfo {
public:
  template <typename V>
  explicit DetectorInfo(V &&instrumentTree)
      : m_nDetectors(instrumentTree->nDetectors()),
        m_isMasked(std::make_shared<MaskFlags>(m_nDetectors, Bool(false))),
        m_isMonitor(std::make_shared<MonitorFlags>(m_nDetectors, Bool(false))),
        m_l2(std::make_shared<L2s>(m_nDetectors)),
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
    initL2();
  }

  // template <typename V>
  // DetectorInfo(const DetectorInfo<V> &other) = default;

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
    detectorRangeCheck(detectorIndex, *m_isMasked);
    (*m_isMasked)[detectorIndex] = true;
  }

  bool isMasked(size_t detectorIndex) const {
    detectorRangeCheck(detectorIndex, *m_isMasked);
    return (*m_isMasked)[detectorIndex];
  }

  void setMonitor(size_t detectorIndex) {
    detectorRangeCheck(detectorIndex, *m_isMonitor);
    (*m_isMonitor)[detectorIndex] = true;
  }

  /* In some use cases scientists decide to use arbitrary
   * detectors or groups of detectors as monitors
   * so lets have the client code tell us which ones are monitors.
   */
  bool isMonitor(size_t detectorIndex) const {
    detectorRangeCheck(detectorIndex, *m_isMonitor);
    return (*m_isMonitor)[detectorIndex];
  }

  void initL2() {
    for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
         ++detectorIndex) {
      const Detector &det = m_instrumentTree->getDetector(detectorIndex);
      auto detPos = det.getPos();

      /*
       * Long-term is is not right. We cannot assume that the L2 path is simply
       * sample
       * to detector.
       */

      (*m_l2)[detectorIndex] = distanceToSample(detPos);
    }
  }

  double l2(size_t detectorIndex) const {
    detectorRangeCheck(detectorIndex, *m_l2);
    return (*m_l2)[detectorIndex];
  }

  V3D position(size_t detectorIndex) const {
    return m_instrumentTree->getDetector(detectorIndex).getPos();
  }

  double l1() const { return m_l1; }
  size_t size() const { return m_nDetectors; }

  const InstTree &const_instrumentTree() const { return *m_instrumentTree; }

  void modify(size_t nodeIndex, Command &command) {
    m_instrumentTree = m_instrumentTree->modify(command);

    // All other geometry-derived information is now also invalid. Very
    // important!
    m_sourcePos = m_instrumentTree->sourcePos();
    m_samplePos = m_instrumentTree->samplePos();
    m_l1 = distance(m_sourcePos, m_samplePos);
    initL2();

    // Meta-data should all still be valid.
  }

  double distanceToSample(const V3D &item) const {
    return distance(item, m_samplePos);
  }

  std::vector<Spectrum> makeSpectra() const {
    std::vector<Spectrum> spectra;
    spectra.reserve(this->size());
    for (size_t i = 0; i < this->size(); ++i) {
      spectra.push_back(i);
    }
    return spectra;
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

  std::shared_ptr<const InstTree> m_instrumentTree;

  //------------------- MetaData -------------
  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked; // This could be copied upon instrument change
  CowPtr<MonitorFlags>
      m_isMonitor; // This could be copied upon instrument change

  //------------------- DerivedInfo
  double m_l1;               // This can't be copied upon instrument change
  V3D m_sourcePos;           // This can't be copied upon instrument change
  V3D m_samplePos;           // This can't be copied upon instrument change
  CowPtr<L2s> m_l2;          // This can't be copied upon instrument change
};

#endif
