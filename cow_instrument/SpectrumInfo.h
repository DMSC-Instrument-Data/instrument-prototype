#ifndef SPECTRUMINFO_H
#define SPECTRUMINFO_H

#include <vector>
#include "Spectrum.h"
#include "DetectorInfo.h"
#include "InstrumentTree.h"

template <typename InstTree> class SpectrumInfo {
public:
  size_t size() { return m_spectra.size(); }

  SpectrumInfo(const std::vector<Spectrum> &spectra,
               const DetectorInfo<InstTree> &detectorInfo)
      : m_spectra(spectra), m_detectorInfo(detectorInfo)
         {}

  size_t size() const { return m_spectra.size(); }

  size_t nDetectors() const {
    size_t count = 0;
    for (const Spectrum &s : m_spectra) {
      count += s.size();
    }
    return count;
  }

  Spectrum spectra(size_t index) { return m_spectra[index]; }



private:
  DetectorInfo<InstTree> m_detectorInfo;
  std::vector<Spectrum> m_spectra;
};

#endif
