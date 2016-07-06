#ifndef SPECTRUM_H
#define SPECTRUM_H
#include "FixedLengthVector.h"
#include <vector>

class Spectrum {
public:
  Spectrum(size_t index);
  Spectrum(std::initializer_list<size_t> init);
  Spectrum(const Spectrum &) = default;
  Spectrum(Spectrum &&) = default;
  Spectrum(const std::vector<size_t> &detectorIndexes);
  Spectrum(std::vector<size_t> &&detectorIndexes);

  Spectrum &operator=(const Spectrum &rhs) = default;
  Spectrum &operator=(Spectrum &&rhs) = default;
  Spectrum &operator=(const std::vector<size_t> &rhs);
  Spectrum &operator=(std::vector<size_t> &&rhs);
  Spectrum &operator=(std::initializer_list<size_t> ilist);
  bool operator==(const Spectrum &other) const;
  bool operator!=(const Spectrum &other) const;
  const std::vector<size_t> &detectorIndexes() const;

  size_t size() const;
  const size_t &operator[](size_t pos) const;
  size_t &operator[](size_t pos);

private:
  std::vector<size_t> m_detectorIndexes;
};

// TODO: Spectra may not have a fixed length.
class Spectra : public FixedLengthVector<Spectra, Spectrum> {
public:
    using FixedLengthVector<Spectra, Spectrum>::FixedLengthVector;
};

#endif
