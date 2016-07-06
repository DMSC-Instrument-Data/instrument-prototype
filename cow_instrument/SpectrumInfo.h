#ifndef SPECTRUMINFO_H
#define SPECTRUMINFO_H

#include <vector>
#include "Spectrum.h"
#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "L2s.h"
#include "cow_ptr.h"

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

template <typename InstTree> class SpectrumInfo {
public:
  size_t size() { return m_spectra->size(); }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:1 spectrum to detector only
   * mapping.
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const DetectorInfo<InstTree> &detectorInfo)
      : m_detectorInfo(detectorInfo), m_l2(detectorInfo.l2s()),
        m_spectra(
            std::make_shared<Spectra>(std::move(detectorInfo.makeSpectra()))) {

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
  SpectrumInfo(const std::vector<Spectrum> &spectra,
               const DetectorInfo<InstTree> &detectorInfo)
      : m_spectra(std::make_shared<Spectra>(spectra.begin(), spectra.end())),
        m_detectorInfo(detectorInfo),
        m_l2(std::make_shared<L2s>(spectra.size())) {
    initL2();
  }

  void initL2() {
    for (size_t spectrumIndex = 0; spectrumIndex < this->size();
         ++spectrumIndex) {

      double l2Temp = 0;
      for (auto detectorIndex :
           m_spectra.const_ref()[spectrumIndex].detectorIndexes()) {
        l2Temp += m_detectorInfo.l2(detectorIndex);
      }
      // Divide through by number of detectors
      l2Temp /= m_spectra.const_ref()[spectrumIndex].size();
      (*m_l2)[spectrumIndex] = l2Temp;
    }
  }

  size_t size() const { return m_spectra->size(); }

  size_t nDetectors() const {
    size_t count = 0;
    for (const Spectrum &s : m_spectra.const_ref()) {
      count += s.size();
    }
    return count;
  }

  Spectrum spectrum(size_t index) const {
    spectraRangeCheck(index, m_spectra.const_ref());
    return m_spectra.const_ref()[index];
  }

  double l2(size_t index) const {
    spectraRangeCheck(index, m_spectra.const_ref());
    return m_l2->operator[](index);
  }

  CowPtr<L2s> l2s() const { return m_l2; }

private:
  DetectorInfo<InstTree> m_detectorInfo;
  CowPtr<Spectra> m_spectra;
  CowPtr<L2s> m_l2;
};

#endif
