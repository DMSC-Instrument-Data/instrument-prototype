#ifndef SPECTRUM_H
#define SPECTRUM_H
#include "FixedLengthVector.h"
#include "VectorOf.h"

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
