#ifndef SPECTRUMINFO_H
#define SPECTRUMINFO_H

#include <vector>
#include "Spectrum.h"
#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "L2s.h"

namespace {

template <typename U>
void spectraRangeCheck(size_t spectraIndex, const U &container) {
  if (spectraIndex >= container.size()) {
    std::stringstream buffer;
    buffer << "Spectra at index " << spectraIndex << " is out of range";
    throw std::out_of_range(buffer.str());
  }
}
}

template <typename InstTree> class SpectrumInfo {
public:
  size_t size() { return m_spectra.size(); }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:1 spectra to detector only
   * mapping.
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const DetectorInfo<InstTree> &detectorInfo)
      : m_detectorInfo(detectorInfo), m_l2(detectorInfo.size()),
        m_spectra(std::move(detectorInfo.makeSpectra())) {

    initL2();
  }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:n spectra to detector
   * mapping.
   * @param spectra : DetectorIndexes to SpectraIndexes
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const std::vector<Spectrum> &spectra,
               const DetectorInfo<InstTree> &detectorInfo)
      : m_spectra(spectra), m_detectorInfo(detectorInfo), m_l2(spectra.size()) {
    initL2();
  }

  void initL2() {
    for (size_t spectraIndex = 0; spectraIndex < this->size(); ++spectraIndex) {

      double l2Temp = 0;
      for (auto detectorIndex : m_spectra[spectraIndex].detectorIndexes()) {
        l2Temp += m_detectorInfo.l2(detectorIndex);
      }
      // Divide through by number of detectors
      l2Temp /= m_spectra[spectraIndex].size();
      m_l2[spectraIndex] = l2Temp;
    }
  }

  size_t size() const { return m_spectra.size(); }

  size_t nDetectors() const {
    size_t count = 0;
    for (const Spectrum &s : m_spectra) {
      count += s.size();
    }
    return count;
  }

  Spectrum spectra(size_t index) {
    spectraRangeCheck(index, m_spectra);
    return m_spectra[index];
  }

  double getL2(size_t index) {
    spectraRangeCheck(index, m_spectra);
    return m_l2[index];
  }

private:
  DetectorInfo<InstTree> m_detectorInfo;
  std::vector<Spectrum> m_spectra;
  L2s m_l2;
};

#endif
