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
  size_t size() { return m_spectrum.size(); }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:1 spectra to detector only
   * mapping.
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const DetectorInfo<InstTree> &detectorInfo)
      : m_detectorInfo(detectorInfo), m_l2(detectorInfo.size()),
        m_spectrum(std::move(detectorInfo.makeSpectra())) {

    initL2();
  }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:n spectra to detector
   * mapping.
   * @param spectrum : DetectorIndexes to spectrumIndexes
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const std::vector<Spectrum> &spectrum,
               const DetectorInfo<InstTree> &detectorInfo)
      : m_spectrum(spectrum), m_detectorInfo(detectorInfo), m_l2(spectrum.size()) {
    initL2();
  }

  void initL2() {
    for (size_t spectrumIndex = 0; spectrumIndex < this->size(); ++spectrumIndex) {

      double l2Temp = 0;
      for (auto detectorIndex : m_spectrum[spectrumIndex].detectorIndexes()) {
        l2Temp += m_detectorInfo.l2(detectorIndex);
      }
      // Divide through by number of detectors
      l2Temp /= m_spectrum[spectrumIndex].size();
      m_l2[spectrumIndex] = l2Temp;
    }
  }

  size_t size() const { return m_spectrum.size(); }

  size_t nDetectors() const {
    size_t count = 0;
    for (const Spectrum &s : m_spectrum) {
      count += s.size();
    }
    return count;
  }

  Spectrum spectra(size_t index) {
    spectraRangeCheck(index, m_spectrum);
    return m_spectrum[index];
  }

  double l2(size_t index) {
    spectraRangeCheck(index, m_spectrum);
    return m_l2[index];
  }

private:
  DetectorInfo<InstTree> m_detectorInfo;
  const std::vector<Spectrum> m_spectrum;
  L2s m_l2;
};

#endif
