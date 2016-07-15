#ifndef DETECTOR_INFO_H
#define DETECTOR_INFO_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cmath>

#include "Command.h"
#include "cow_ptr.h"
#include "Detector.h"
#include "IdType.h"
#include "L2s.h"
#include "MaskFlags.h"
#include "MonitorFlags.h"
#include "Path.h"
#include "Spectrum.h"

/**
 * DetectorInfo type. Provides Meta-data context to an InstrumentTree
 * of detectors, and a facade for modifications.
 * Meta-data is provided per detector.
 */
template <typename InstTree> class DetectorInfo {
public:
  template <typename V> explicit DetectorInfo(V &&instrumentTree);

  void setMasked(size_t detectorIndex);

  bool isMasked(size_t detectorIndex) const;

  void setMonitor(size_t detectorIndex);

  bool isMonitor(size_t detectorIndex) const;

  void initL2();

  double l2(size_t detectorIndex) const;

  V3D position(size_t detectorIndex) const;

  double l1() const;

  size_t size() const;

  const InstTree &const_instrumentTree() const;

  void modify(size_t nodeIndex, Command &command);

  double distanceToSample(const V3D &item) const;

  std::vector<Spectrum> makeSpectra() const;

  CowPtr<L2s> l2s() const;

private:
  std::shared_ptr<const InstTree> m_instrumentTree;

  //------------------- MetaData -------------
  const size_t m_nDetectors;
  CowPtr<MaskFlags> m_isMasked; // This could be copied upon instrument change
  CowPtr<MonitorFlags>
      m_isMonitor; // This could be copied upon instrument change

  //------------------- DerivedInfo
  double m_l1;
  V3D m_sourcePos;
  V3D m_samplePos;
  CowPtr<L2s> m_l2;
};

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

template <typename InstTree>
template <typename V>
DetectorInfo<InstTree>::DetectorInfo(V &&instrumentTree)
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

template <typename InstTree>
void DetectorInfo<InstTree>::setMasked(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  (*m_isMasked)[detectorIndex] = true;
}

template <typename InstTree>
bool DetectorInfo<InstTree>::isMasked(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_isMasked.const_ref());
  return m_isMasked.const_ref()[detectorIndex];
}

template <typename InstTree>
void DetectorInfo<InstTree>::setMonitor(size_t detectorIndex) {
  detectorRangeCheck(detectorIndex, m_isMonitor.const_ref());
  (*m_isMonitor)[detectorIndex] = true;
}

/* In some use cases scientists decide to use arbitrary
 * detectors or groups of detectors as monitors
 * so lets have the client code tell us which ones are monitors.
 */

template <typename InstTree>
bool DetectorInfo<InstTree>::isMonitor(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_isMonitor.const_ref());
  return m_isMonitor.const_ref()[detectorIndex];
}

template <typename InstTree> void DetectorInfo<InstTree>::initL2() {
  for (size_t detectorIndex = 0; detectorIndex < m_nDetectors;
       ++detectorIndex) {
    const Detector &det = m_instrumentTree->getDetector(detectorIndex);
    auto detPos = det.getPos();

    /*
     * Long-term this is not right. We cannot assume that the L2 path is simply
     * sample
     * to detector.
     */

    (*m_l2)[detectorIndex] = distanceToSample(detPos);
  }
}

template <typename InstTree>
double DetectorInfo<InstTree>::l2(size_t detectorIndex) const {
  detectorRangeCheck(detectorIndex, m_l2.const_ref());
  return m_l2.const_ref()[detectorIndex];
}

template <typename InstTree>
V3D DetectorInfo<InstTree>::position(size_t detectorIndex) const {
  return m_instrumentTree->getDetector(detectorIndex).getPos();
}

template <typename InstTree> double DetectorInfo<InstTree>::l1() const {
  return m_l1;
}

template <typename InstTree> size_t DetectorInfo<InstTree>::size() const {
  return m_nDetectors;
}

template <typename InstTree>
const InstTree &DetectorInfo<InstTree>::const_instrumentTree() const {
  return *m_instrumentTree;
}

template <typename InstTree>
void DetectorInfo<InstTree>::modify(size_t nodeIndex, Command &command) {

  m_instrumentTree->modify(nodeIndex, command);

  // All other geometry-derived information is now also invalid. Very
  // important!
  m_sourcePos = m_instrumentTree->sourcePos();
  m_samplePos = m_instrumentTree->samplePos();
  m_l1 = distance(m_sourcePos, m_samplePos);
  initL2();

  // Meta-data should all still be valid.
}

template <typename InstTree>
double DetectorInfo<InstTree>::distanceToSample(const V3D &item) const {
  return distance(item, m_samplePos);
}

template <typename InstTree>
std::vector<Spectrum> DetectorInfo<InstTree>::makeSpectra() const {
  std::vector<Spectrum> spectra;
  spectra.reserve(this->size());
  for (size_t i = 0; i < this->size(); ++i) {
    spectra.push_back(i);
  }
  return spectra;
}

template <typename InstTree> CowPtr<L2s> DetectorInfo<InstTree>::l2s() const {
  return m_l2;
}

#endif
