#ifndef SPECTRUMINFO_H
#define SPECTRUMINFO_H

#include <vector>
#include "Spectrum.h"
#include "DetectorInfo.h"
#include "InstrumentTree.h"
#include "L2s.h"

template <typename InstTree> class SpectrumInfo {
public:
  size_t size() { return m_spectra.size(); }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:1 spectra to detector only
   * mapping.
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const DetectorInfo<InstTree> &detectorInfo)
      : m_detectorInfo(detectorInfo), m_l2Flags(detectorInfo.size()),
        m_l2(detectorInfo.size()),
        m_spectra(std::move(detectorInfo.makeSpectra())) {

    // TODO. Meta-data arrays can just reference the same cow ptrs as used in
    // detector_info.

  }

  /**
   * @brief SpectrumInfo constructor. Suitable for 1:n spectra to detector
   * mapping.
   * @param spectra : DetectorIndexes to SpectraIndexes
   * @param detectorInfo : DetectorInfo object
   */
  SpectrumInfo(const std::vector<Spectrum> &spectra,
               const DetectorInfo<InstTree> &detectorInfo)
      : m_spectra(spectra), m_detectorInfo(detectorInfo),
        m_l2Flags(spectra.size()), m_l2(spectra.size()) {}

  size_t size() const { return m_spectra.size(); }

  size_t nDetectors() const {
    size_t count = 0;
    for (const Spectrum &s : m_spectra) {
      count += s.size();
    }
    return count;
  }

  Spectrum spectra(size_t index) { return m_spectra[index]; }

  double getL2(size_t index) {
    if (!m_l2Flags[index]) {
      V3D temp;
      for (auto detectorIndex : m_spectra[index].detectorIndexes()) {
        auto pos = m_detectorInfo.position(detectorIndex);
        temp[0] += pos[0];
        temp[1] += pos[1];
        temp[2] += pos[2];
      }
      // Divide through by number of detectors
      temp[0] /= m_spectra[index].size();
      temp[1] /= m_spectra[index].size();
      temp[2] /= m_spectra[index].size();
      m_l2[index] = m_detectorInfo.distanceToSample(temp);
    }
    return m_l2[index];
  }

private:
  DetectorInfo<InstTree> m_detectorInfo;
  std::vector<Spectrum> m_spectra;
  L2s m_l2;
  L2Flags m_l2Flags;
};

#endif
