#ifndef INDEXTRANSLATOR_H
#define INDEXTRANSLATOR_H

#include <stdint.h>
#include <stddef.h>
#include <stdexcept>
#include <vector>

// Ultimately, these should not just be typedefs but distinct types.
using SpectrumNumber = int32_t;
using GlobalSpectrumIndex = size_t;

class Partitioning {
public:
  Partitioning(int numberOfPartitions)
      : m_numberOfPartitions(numberOfPartitions) {}

  int size() const { return m_numberOfPartitions; }

  int partitionIndexOf(const GlobalSpectrumIndex index) const {
    return index % m_numberOfPartitions;
  }

private:
  int m_numberOfPartitions;
};

class IndexTranslator {
public:
  IndexTranslator(const Partitioning &partitioning, int partition,
                  const std::vector<SpectrumNumber> &spectrumNumbers)
      : m_partitioning(partitioning), m_partition(partition) {
    for (size_t i = 0; i < spectrumNumbers.size(); ++i) {
      int partition = m_partitioning.partitionIndexOf(i);
      if (partition == m_partition) {
        m_spectrumNumbers.push_back(spectrumNumbers[i]);
        m_globalSpectrumIndices.push_back(i);
      }
    }
  }

  std::vector<std::vector<size_t>>
  spectrumIndices(const Partitioning targetPartitioning) const {
    std::vector<std::vector<size_t>> tmp(targetPartitioning.size());
    for (size_t i = 0; i < m_globalSpectrumIndices.size(); ++i)
      tmp[targetPartitioning.partitionIndexOf(m_globalSpectrumIndices[i])]
          .push_back(i);
    return tmp;
  }

  std::vector<std::vector<GlobalSpectrumIndex>>
  globalSpectrumIndices(const Partitioning targetPartitioning) const {
    std::vector<std::vector<GlobalSpectrumIndex>> tmp(
        targetPartitioning.size());
    for (const auto &index : m_globalSpectrumIndices)
      tmp[targetPartitioning.partitionIndexOf(index)].push_back(index);
    return tmp;
  }

  std::vector<std::vector<SpectrumNumber>>
  spectrumNumbers(const Partitioning targetPartitioning) const {
    std::vector<std::vector<SpectrumNumber>> tmp(targetPartitioning.size());
    for (size_t i = 0; i < m_spectrumNumbers.size(); ++i)
      tmp[targetPartitioning.partitionIndexOf(m_globalSpectrumIndices[i])]
          .push_back(m_spectrumNumbers[i]);
    return tmp;
  }

private:
  Partitioning m_partitioning;
  int m_partition;
  std::vector<SpectrumNumber>
      m_spectrumNumbers; // Spectrum numbers of all local spectra
  std::vector<GlobalSpectrumIndex>
      m_globalSpectrumIndices; // Global indices of all local spectra
};

#endif // INDEXTRANSLATOR_H
