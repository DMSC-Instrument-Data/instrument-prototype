#include "Spectrum.h"

Spectrum::Spectrum(std::initializer_list<size_t> init)
    : m_detectorIndexes(init) {}

Spectrum::Spectrum(const std::vector<size_t> &detectorIndexes)
    : m_detectorIndexes(detectorIndexes) {}

Spectrum::Spectrum(std::vector<size_t> &&detectorIndexes)
    : m_detectorIndexes(std::move(detectorIndexes)) {}

Spectrum &Spectrum::operator=(const std::vector<size_t> &rhs) {
  m_detectorIndexes = rhs;
  return *this;
}

Spectrum &Spectrum::operator=(std::vector<size_t> &&rhs) {
  m_detectorIndexes = std::move(rhs);
  return *this;
}

Spectrum &Spectrum::operator=(std::initializer_list<size_t> ilist) {
  m_detectorIndexes = ilist;
  return *this;
}

bool Spectrum::operator==(const Spectrum &other) const {
  return m_detectorIndexes == other.m_detectorIndexes;
}

bool Spectrum::operator!=(const Spectrum &other) const {
  return m_detectorIndexes != other.m_detectorIndexes;
}

const std::vector<size_t> &Spectrum::detectorIndexes() const {
  return m_detectorIndexes;
}

size_t Spectrum::size() const { return m_detectorIndexes.size(); }

const size_t &Spectrum::operator[](size_t pos) const {
  return m_detectorIndexes[pos];
}

size_t &Spectrum::operator[](size_t pos) { return m_detectorIndexes[pos]; }
