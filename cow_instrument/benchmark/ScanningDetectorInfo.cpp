#include "StandardBenchmark.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "DetectorInfo.h"
#include "PointSample.h"
#include "PointSource.h"
#include "SourceSampleDetectorPathFactory.h"
#include "FlatTree.h"
#include "ScanTime.h"
#include <benchmark/benchmark_api.h>
#include <iostream>

namespace {

std::unique_ptr<Component> make_tube(size_t nPixels, std::string name,
                                     DetectorIdType detectorId,
                                     ComponentIdType componentId) {
  std::unique_ptr<CompositeComponent> tube(
      new CompositeComponent(ComponentIdType(0), name));

  for (size_t i = 0; i < nPixels; ++i) {
    tube->addComponent(std::unique_ptr<DetectorComponent>(
        new DetectorComponent(componentId++, detectorId++, Eigen::Vector3d{})));
  }

  return std::move(tube);
}

/*
 * Analogue to D2B
 */
DetectorInfo<FlatTree> createScanningDetectorInfo() {
  /*
          instrument root
          |
   ------------------------------------
    |         |                |
 source     sample           tubes

   64 physical tubes.
   256 detectors per tube
   100 scans.
   1638400 effective pixels on instrument

  */

  const size_t nTubePixels = 256;
  const size_t nScans = 100;
  const size_t nTubes = 64;
  const size_t nTimePixels = nTubePixels * nScans * nTubes;

  ScanTimes scanTimes;
  for (size_t t = 0; t < nScans; ++t) {
    scanTimes.push_back(ScanTime{std::time_t(t), t + 1}); // 100 time intervals
  }

  // Make root
  auto root = std::make_shared<CompositeComponent>(ComponentIdType(0), "root");

  // Add source
  root->addComponent(std::move(std::unique_ptr<PointSource>(
      new PointSource{Eigen::Vector3d{0, 0, 0}, ComponentIdType(1)})));

  // Add sample
  root->addComponent(std::move(std::unique_ptr<PointSample>(
      new PointSample{Eigen::Vector3d{0, 0, 10}, ComponentIdType(2)})));

  // Add 64 tubes
  for (size_t i = 0; i < nTubes; ++i) {

    root->addComponent(make_tube(nTubePixels,
                                 std::string("tube ") + std::to_string(i),
                                 DetectorIdType(i), ComponentIdType(i + 3)));
  }

  auto positions = std::vector<Eigen::Vector3d>();
  positions.reserve(nTimePixels);

  auto rotations = std::vector<Eigen::Quaterniond>(
      nTimePixels, Eigen::Quaterniond{Eigen::Affine3d::Identity().rotation()});
  std::vector<std::vector<size_t>> timeIndexes(nTubePixels * nTubes);

  // initialize positions and time indexes
  for (size_t i = 0; i < timeIndexes.size(); ++i) {

    double x = i;
    std::vector<size_t> inner(nScans);
    for (size_t s = 0; s < nScans; ++s) {
      x += 1.0 / static_cast<double>(nScans);

      positions.push_back(Eigen::Vector3d{x, 0, 0});

      inner[s] = (i * nScans) + s;
    }
    timeIndexes[i] = inner;
  }

  std::shared_ptr<FlatTree> instrument =
      std::make_shared<FlatTree>(std::move(root));

  DetectorInfo<FlatTree> info(instrument, timeIndexes, scanTimes, positions,
                              rotations);

  return info;
}

class DetectorInfoScanningFixture
    : public StandardBenchmark<DetectorInfoScanningFixture> {
private:
  DetectorInfo<FlatTree> m_detectorInfo;

public:
  DetectorInfoScanningFixture()
      : m_detectorInfo(createScanningDetectorInfo()) {}

  void doConstruct(benchmark::State &state) {

    while (state.KeepRunning()) {

      benchmark::DoNotOptimize(createScanningDetectorInfo());
    }
    state.SetItemsProcessed(state.iterations() * 1);
  }

  void doCopy(benchmark::State &state) {
    while (state.KeepRunning()) {
      auto copy = m_detectorInfo;
      (void)copy;
    }
    state.SetItemsProcessed(state.iterations() * 1);
  }

  void doReadPositions(benchmark::State &state) {
    size_t detectorSize = m_detectorInfo.detectorSize();
    size_t scanSize = m_detectorInfo.scanCount();

    while (state.KeepRunning()) {
      for (size_t i = 0; i < detectorSize; ++i) {
        for (size_t t = 0; t < scanSize; ++t) {
          benchmark::DoNotOptimize(m_detectorInfo.position(i, t));
        }
      }
    }
    state.SetItemsProcessed(state.iterations() * detectorSize * scanSize);
  }

  void doReadL2s(benchmark::State &state) {

    size_t detectorSize = m_detectorInfo.detectorSize();
    size_t scanSize = m_detectorInfo.scanCount();

    while (state.KeepRunning()) {
      for (size_t i = 0; i < detectorSize; ++i) {
        for (size_t t = 0; t < scanSize; ++t) {
          benchmark::DoNotOptimize(m_detectorInfo.l2(i, t));
        }
      }
    }
    state.SetItemsProcessed(state.iterations() * detectorSize * scanSize);
  }
};
BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_construct)(benchmark::State &state) {

  doConstruct(state);
}
BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_copy)(benchmark::State &state) {

  doCopy(state);
}
BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_read_positions)(benchmark::State &state) {

  // Find out how quickly we can read positions.
  doReadPositions(state);
}

BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_read_l2s)(benchmark::State &state) {

  // Find out how quickly we can read l2.
  doReadL2s(state);
}
}
