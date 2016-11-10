#include "StandardInstrument.h"
#include "CompositeComponent.h"
#include "cow_ptr.h"
#include "DetectorComponent.h"
#include "Node.h"
#include "NullComponent.h"
#include "PointSample.h"
#include "PointSource.h"
#include "SourceSampleDetectorPathFactory.h"

namespace {

std::shared_ptr<CompositeComponent>
make_square_bank(size_t width, size_t height, std::string name) {
  static DetectorIdType detectorId(1);
  static ComponentIdType componentId(1);
  auto bank = std::make_shared<CompositeComponent>(ComponentIdType(0), name);

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      bank->addComponent(std::unique_ptr<DetectorComponent>(
          new DetectorComponent(componentId++, detectorId++,
                                Eigen::Vector3d{double(i), double(j), double(0)})));
    }
  }
  bank->shiftPositionBy(
      Eigen::Vector3d{-double(width) / 2, -double(height) / 2, 0}); // Center it
  return bank;
}

std::unique_ptr<Component> make_square_bank2(size_t width, size_t height,
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
  bank->shiftPositionBy(
      Eigen::Vector3d{-double(width) / 2, -double(height) / 2, 0}); // Center it
  return std::move(bank);
}
}

namespace std_instrument {

std::vector<Node> construct_root_node() {
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
  const double width_d = double(width);
  const double height_d = double(height);

  auto N = make_square_bank(width, height, "North");
  N->shiftPositionBy(Eigen::Vector3d{0, height_d, 3});
  auto E = make_square_bank(width, height, "South");
  E->shiftPositionBy(Eigen::Vector3d{-width_d, 0, 3});
  auto S = make_square_bank(width, height, "East");
  S->shiftPositionBy(Eigen::Vector3d{0, -height_d, 3});
  auto W = make_square_bank(width, height, "West");
  E->shiftPositionBy(Eigen::Vector3d{width_d, 0, 3});

  auto l_curtain = make_square_bank(width, height, "Left curtain");
  l_curtain->shiftPositionBy(Eigen::Vector3d{-width_d, 0, 6});
  auto r_curtain = make_square_bank(width, height, "Right curtain");
  r_curtain->shiftPositionBy(Eigen::Vector3d{width_d, 0, 6});

  std::vector<Node> nodes;
  nodes.emplace_back(CowPtr<Component>(new NullComponent), "Root node");
  nodes.emplace_back(CowPtr<Component>(new NullComponent),
                     "Front trolley node");
  nodes.emplace_back(CowPtr<Component>(N), "North node");
  nodes.emplace_back(CowPtr<Component>(E), "East node");
  nodes.emplace_back(CowPtr<Component>(S), "South node");
  nodes.emplace_back(CowPtr<Component>(W), "West node");
  nodes.emplace_back(CowPtr<Component>(new NullComponent), "Rear trolley node");
  nodes.emplace_back(CowPtr<Component>(l_curtain), "Left curtain node");
  nodes.emplace_back(CowPtr<Component>(l_curtain), "Left curtain node");
  nodes.emplace_back(
      CowPtr<Component>(new PointSource(Eigen::Vector3d{0, 0, 0}, ComponentIdType(100))));
  nodes.emplace_back(
      CowPtr<Component>(new PointSample(Eigen::Vector3d{0, 0, 10}, ComponentIdType(100))));

  // Assemble flattened node node tree
  nodes[0].addChild(1);
  nodes[0].addChild(6);
  nodes[0].addChild(9);
  nodes[0].addChild(10);
  nodes[1].addChild(2);
  nodes[1].addChild(3);
  nodes[1].addChild(4);
  nodes[1].addChild(5);
  nodes[6].addChild(7);
  nodes[6].addChild(8);

  return nodes;
}

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
  const double width_d = double(width);
  const double height_d = double(height);

  auto N = make_square_bank2(width, height, "North");
  N->shiftPositionBy(Eigen::Vector3d{0, height_d, 3});
  auto E = make_square_bank2(width, height, "South");
  E->shiftPositionBy(Eigen::Vector3d{-width_d, 0, 3});
  auto S = make_square_bank2(width, height, "East");
  S->shiftPositionBy(Eigen::Vector3d{0, -height_d, 3});
  auto W = make_square_bank2(width, height, "West");
  E->shiftPositionBy(Eigen::Vector3d{width_d, 0, 3});

  auto l_curtain = make_square_bank2(width, height, "Left curtain");
  l_curtain->shiftPositionBy(Eigen::Vector3d{-width_d, 0, 6});
  auto r_curtain = make_square_bank2(width, height, "Right curtain");
  r_curtain->shiftPositionBy(Eigen::Vector3d{width_d, 0, 6});

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
    : benchmark::Fixture(), m_instrument(std_instrument::construct_root_node()),
      m_detectorInfo(std::make_shared<InstrumentTree>(
                         std_instrument::construct_root_component()),
                     SourceSampleDetectorPathFactory<InstrumentTree>{}) {}
