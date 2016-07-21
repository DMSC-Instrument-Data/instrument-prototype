#ifndef SPECTRUMINFO_H
#define SPECTRUMINFO_H

#include <vector>
#include <memory>

#include "Command.h"
#include "cow_ptr.h"
#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "L2s.h"
#include "Spectrum.h"

/**
 * SpectrumInfo. Provides a spectrum centric Facade over DetectorInfo.
 * Internally handles Spectrum-Detector mapping. Provides derived quanitites
 * at the Spectrum level, not at the detector level.
 */
template <typename InstTree> class SpectrumInfo {
public:
  SpectrumInfo(const DetectorInfo<InstTree> &detectorInfo);
  SpectrumInfo(DetectorInfo<InstTree> &&detectorInfo);

  SpectrumInfo(const std::vector<Spectrum> &spectra,
               const DetectorInfo<InstTree> &detectorInfo);

  void initL2();

  size_t size() const;

  size_t nDetectors() const;

  Spectrum spectrum(size_t index) const;

  double l2(size_t index) const;

  CowPtr<L2s> l2s() const;

  void modify(size_t nodeIndex, Command &command);

private:
  DetectorInfo<InstTree> m_detectorInfo;
  CowPtr<Spectra> m_spectra;
  CowPtr<L2s> m_l2;
};

namespace {

template <typename U>
void spectraRangeCheck(size_t spectrumIndex, const U &container) {
  if (spectrumIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "Spectra at index " << spectrumIndex << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}
}

/**
 * @brief SpectrumInfo constructor. Suitable for 1:1 spectrum to detector only
 * mapping.
 * @param detectorInfo : DetectorInfo object
 */
template <typename InstTree>
SpectrumInfo<InstTree>::SpectrumInfo(const DetectorInfo<InstTree> &detectorInfo)
    : m_detectorInfo(detectorInfo), m_l2(detectorInfo.l2s()),
      m_spectra(
          std::make_shared<Spectra>(std::move(m_detectorInfo.makeSpectra()))) {

  /* 1:1 mapping. Meta-data arrays can be
   * referenced directly from DetectorInfo via COW mechanism.
   */
}

template <typename InstTree>
SpectrumInfo<InstTree>::SpectrumInfo(DetectorInfo<InstTree> &&detectorInfo)
    : m_detectorInfo(std::move(detectorInfo)), m_l2(m_detectorInfo.l2s()),
      m_spectra(
          std::make_shared<Spectra>(std::move(m_detectorInfo.makeSpectra()))) {

  /* 1:1 mapping. Meta-data arrays can be
   * referenced directly from DetectorInfo via COW mechanism.
   */
}

/**
 * @brief SpectrumInfo constructor. Suitable for 1:n spectrum to detector
 * mapping.
 * @param spectra : DetectorIndexes to spectrumIndexes
 * @param detectorInfo : DetectorInfo object
 */
template <typename InstTree>
SpectrumInfo<InstTree>::SpectrumInfo(const std::vector<Spectrum> &spectra,
                                     const DetectorInfo<InstTree> &detectorInfo)
    : m_spectra(std::make_shared<Spectra>(spectra.begin(), spectra.end())),
      m_detectorInfo(detectorInfo),
      m_l2(std::make_shared<L2s>(spectra.size())) {
  initL2();
}

template <typename InstTree> void SpectrumInfo<InstTree>::initL2() {
  for (size_t spectrumIndex = 0; spectrumIndex < this->size();
       ++spectrumIndex) {

    double l2Temp = 0;
    for (auto detectorIndex : m_spectra.const_ref()[spectrumIndex].indexes()) {
      l2Temp += m_detectorInfo.l2(detectorIndex);
    }
    // Divide through by number of detectors
    l2Temp /= m_spectra.const_ref()[spectrumIndex].size();
    (*m_l2)[spectrumIndex] = l2Temp;
  }
}

template <typename InstTree> size_t SpectrumInfo<InstTree>::size() const {
  return m_spectra->size();
}

template <typename InstTree> size_t SpectrumInfo<InstTree>::nDetectors() const {
  size_t count = 0;
  for (const Spectrum &s : m_spectra.const_ref()) {
    count += s.size();
  }
  return count;
}

template <typename InstTree>
Spectrum SpectrumInfo<InstTree>::spectrum(size_t index) const {
  spectraRangeCheck(index, m_spectra.const_ref());
  return m_spectra.const_ref()[index];
}

template <typename InstTree>
double SpectrumInfo<InstTree>::l2(size_t index) const {
  spectraRangeCheck(index, m_spectra.const_ref());
  return m_l2->operator[](index);
}

template <typename InstTree> CowPtr<L2s> SpectrumInfo<InstTree>::l2s() const {
  return m_l2;
}

template <typename InstTree>
void SpectrumInfo<InstTree>::modify(size_t nodeIndex, Command &command) {
  m_detectorInfo.modify(nodeIndex, command);
}

#endif
