
#include "InstrumentTree.h"
#include "Command.h"

class DetectorPurgingCommand : public Command {

public:
  DetectorPurgingCommand(const std::vector<size_t> &detectorIndexes,
                         const InstrumentTree &instTree) {

    m_detectorIdsToCull(detectorIndexes.size());
    size_t i = 0;
    for (auto index : detectorIndexes) {
      m_detectorIdsToCull[i] = instTree.getDetector(index)->detectorId();
      ++i;
    }
  }

  bool execute(CowPtr<Component> &component) const {

    // This is not a COW operation. That way we avoid automatically forcing a
    // copy when we have no need to.
    auto toCull = component.componentIndexesToCull(m_detectorIdsToCull);

    if (!toCull.empty()) {

      /* Now we invoke COW but we are going to remove everything
      we need from this component in one shot
      */
      component->remove(toCull);
    }
  }

  bool isMetaDataCommand() const {
    return false; // Cascading behaviour is wanted.
  }

private:
  std::vector<size_t> m_detectorIdsToCull;
};

// std::vector<SpectrumInfo> spectrumInfo.split(partitioning);
// MPI_Send(spectrumInfo[rank], rank ...)

std::vector<SpectrumInfo> SpectrumInfo::split(partitioning) {
  //....

  std::vector<std::vector<size_t>> spectrumIndexesOnRanks(
      partitioning.nPartitions());
  for (size_t spectrumIndex = 0; spectrumIndex < this->nSpectra();
       ++spectrumInde) {

    size_t rank = partitioning.rankOfSpectrum(spectrumIndex);
    spectrumIndexesOnRanks[rank].push_back(spectrumIndex);
  }

  std::vector<SpectrumInfo> splits;
  for (auto spectrumIndexesOnRank : spectrumIndexesOnRanks) {

    auto detectorIndexesToKeep =
        this->getDetectorIndexes(spectrumIndexesOnRank);

    auto copyTree = m_instrumentTree;

    DetectorPurgingCommand purgeDetectors(detectorIndexesToKeep, copyTree);
    // Purge detectors from each node in one shot.
    copyTree->modify(purgeDetectors);

    splits.push_back(std::make_shared<DetectorInfo>(copyTree));
  }

  return splits;
}
