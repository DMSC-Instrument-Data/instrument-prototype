#ifndef SPECTRUM_H
#define SPECTRUM_H
#include "FixedLengthVector.h"
#include "VectorOf.h"

/**
 * Spectrum is a vector of detector indexes.
 *
 * One Spectrum per spectrum index in the SpectrumInfo.
 *
 * Stored indexes are the detector indexes in the instrument tree.
 */
class Spectrum : public VectorOf<Spectrum> {
public:
  using VectorOf<Spectrum>::VectorOf;
};

// TODO: Spectra may not have a fixed length.
class Spectra : public FixedLengthVector<Spectra, Spectrum> {
public:
  using FixedLengthVector<Spectra, Spectrum>::FixedLengthVector;
};

#endif
