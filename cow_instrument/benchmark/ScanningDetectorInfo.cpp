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

std::unique_ptr<Component> make_tube(size_t width, size_t height,
                                     std::string name,
                                     DetectorIdType detectorId,
                                     ComponentIdType componentId,
                                     Eigen::Vector3d position) {
  std::unique_ptr<CompositeComponent> tube(
      new CompositeComponent(ComponentIdType(0), name));

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      tube->addComponent(std::unique_ptr<DetectorComponent>(
          new DetectorComponent(componentId++, detectorId++, position)));
    }
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
   100 scans.
   6400 effective tubes on instrument

  */

  const size_t width = 1;
  const size_t height = 256;

  ScanTimes scanTimes;
  for (size_t t; t < 100; ++t) {
    scanTimes.push_back(ScanTime{std::time_t(t), t + 1}); // 100 time intervals
  }

  auto root = std::make_shared<CompositeComponent>(ComponentIdType(0), "root");

  root->addComponent(std::move(std::unique_ptr<PointSource>(
      new PointSource{Eigen::Vector3d{0, 0, 0}, ComponentIdType(1)})));
  root->addComponent(std::move(std::unique_ptr<PointSample>(
      new PointSample{Eigen::Vector3d{0, 0, 10}, ComponentIdType(2)})));

  std::vector<size_t> timeIndexes;
  for (size_t i = 0; i < 64; ++i) {
    double x = i;
    for (size_t s = 0; s < 100; ++s) {
      x += 1 / 100;
      root->addComponent(make_tube(width, height,
                                   std::string("tube ") + std::to_string(i),
                                   DetectorIdType(i), ComponentIdType(i + 3),
                                   Eigen::Vector3d{x, 0.0, 20.0}));
      timeIndexes.push_back(s);
    }
  }

  std::shared_ptr<FlatTree> instrument =
      std::make_shared<FlatTree>(std::move(root));

  DetectorInfo<FlatTree> info(
      instrument, SourceSampleDetectorPathFactory<FlatTree>{},
      std::make_shared<std::vector<size_t>>(timeIndexes),
      std::make_shared<ScanTimes>(scanTimes));
  return info;
}

class DetectorInfoScanningFixture : public benchmark::Fixture {
private:
  DetectorInfo<FlatTree> m_detectorInfo;

public:
  DetectorInfoScanningFixture()
      : m_detectorInfo(createScanningDetectorInfo()) {}

  void doCopy(benchmark::State &state) {
    while (state.KeepRunning()) {
      auto copy = m_detectorInfo;
      (void)copy;
    }
    state.SetItemsProcessed(state.iterations() * 1);
  }

  void doReadDurations(benchmark::State &state) {
    while (state.KeepRunning()) {
      for (size_t i = 0; i < m_detectorInfo.componentSize(); ++i) {
        benchmark::DoNotOptimize(m_detectorInfo.componentDuration(i));
      }
    }
    state.SetItemsProcessed(state.iterations() *
                            m_detectorInfo.componentSize());
  }

  void doFillDetectorMap(benchmark::State &state) {

    while (state.KeepRunning()) {
      std::map<DetectorIdType, std::vector<size_t>> map;
      m_detectorInfo.fillDetectorMap(map);
    }
    state.SetItemsProcessed(state.iterations() * 1);
  }

  void doFillComponentMap(benchmark::State &state) {
    while (state.KeepRunning()) {
      std::map<ComponentIdType, std::vector<size_t>> map;
      m_detectorInfo.fillComponentMap(map);
    }
    state.SetItemsProcessed(state.iterations() * 1);
  }
};

BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_read_durations)(benchmark::State &state) {

  // Find out how quickly we can read durations.
  doReadDurations(state);
}

BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_fill_detector_map)(
    benchmark::State &state) {

  // Find out how quickly we can populate a detector map
  doFillDetectorMap(state);
}

BENCHMARK_F(DetectorInfoScanningFixture,
            BM_detector_info_scanning_fill_component_map)(
    benchmark::State &state) {

  // Find out how quickly we can populate a component map
  doFillComponentMap(state);
}
}
