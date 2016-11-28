#include "StandardInstrument.h"
#include "CompositeComponent.h"
#include "DetectorComponent.h"
#include "PointSample.h"
#include "PointSource.h"
#include "SourceSampleDetectorPathFactory.h"

namespace {

std::unique_ptr<Component> make_square_bank(size_t width, size_t height,
                                            std::string name) {
  static DetectorIdType detectorId(1);
  static ComponentIdType componentId(1);
  std::unique_ptr<CompositeComponent> bank(
      new CompositeComponent(ComponentIdType(0), name));

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      bank->addComponent(
          std::unique_ptr<DetectorComponent>(new DetectorComponent(
              componentId++, detectorId++,
              Eigen::Vector3d{double(i), double(j), double(0)})));
    }
  }

  return std::move(bank);
}
}

namespace std_instrument {

std::shared_ptr<Component> construct_root_component() {
  /*
          instrument
          |
   -----------------------------------------------------------------------
                      |                      |                |         |
                front_trolley           rear_trolley        source    sample
                          |                       |
               ________________________       ________________________
              |       |       |       |       |                     |
              N       S       E       W       l_curtain            r_curtain
  */

  const size_t width = 100;
  const size_t height = 100;

  auto N = make_square_bank(width, height, "North");
  auto E = make_square_bank(width, height, "South");
  auto S = make_square_bank(width, height, "East");
  auto W = make_square_bank(width, height, "West");

  auto l_curtain = make_square_bank(width, height, "Left curtain");
  auto r_curtain = make_square_bank(width, height, "Right curtain");

  auto frontTrolley = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(1), "front_trolley"));
  frontTrolley->addComponent(std::move(N));
  frontTrolley->addComponent(std::move(S));
  frontTrolley->addComponent(std::move(E));
  frontTrolley->addComponent(std::move(W));

  auto rearTrolley = std::unique_ptr<CompositeComponent>(
      new CompositeComponent(ComponentIdType(2), "rear_trolley"));
  rearTrolley->addComponent(std::move(l_curtain));
  rearTrolley->addComponent(std::move(r_curtain));

  auto source = std::unique_ptr<PointSource>(
      new PointSource{Eigen::Vector3d{0, 0, 0}, ComponentIdType(100)});
  auto sample = std::unique_ptr<PointSample>(
      new PointSample{Eigen::Vector3d{0, 0, 10}, ComponentIdType(101)});

  auto root = std::make_shared<CompositeComponent>(ComponentIdType(0), "root");
  root->addComponent(std::move(frontTrolley));
  root->addComponent(std::move(rearTrolley));
  root->addComponent(std::move(source));
  root->addComponent(std::move(sample));

  return root;
}
}

StandardInstrumentFixture::StandardInstrumentFixture()
    : BenchmarkPolicy<StandardInstrumentFixture>(),
      m_instrument(std_instrument::construct_root_component()),
      m_detectorInfo(std::make_shared<FlatTree>(
                         std_instrument::construct_root_component()),
                     SourceSampleDetectorPathFactory<FlatTree>{}) {}
