#ifndef SPECTRUMINFOFACTORY_H
#define SPECTRUMINFOFACTORY_H

#include <memory>
#include "SpectrumInfo.h"
#include "Spectrum.h"

class Partitioning {};

class Spectra

    template <typename InstTree>
    class SpectrumInfoFactory {
public:
  std::unique_ptr<SpectrumInfo<InstTree>>
  create(const Partitioning &partitioning, size_t rank, const Spectra &spectra,
         const L2s &l2s) const;
};

template <typename InstTree>
std::unique_ptr<SpectrumInfo<InstTree>>
SpectrumInfoFactory<InstTree>::create(const Partitioning &partitioning,
                                      size_t rank, const Spectra &spectra,
                                      const L2s &l2s) const {}

#endif
