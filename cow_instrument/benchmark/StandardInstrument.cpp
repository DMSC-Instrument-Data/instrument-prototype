#include "StandardInstrument.h"
#include "CompositeComponent.h"
#include "cow_ptr.h"
#include "DetectorComponent.h"
#include "Node.h"
#include "NullComponent.h"

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
                                V3D{double(i), double(j), double(0)})));
    }
  }
  bank->deltaPos(V3D{-double(width) / 2, -double(height) / 2, 0}); // Center it
  return bank;
}
}

namespace std_instrument {

std::vector<Node> construct_root_node() {
  /*
          instrument
          |
   --------------------------------------------
                      |                      |
                front_trolley           rear_trolley
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
  N->deltaPos(V3D{0, height_d, 3});
  auto E = make_square_bank(width, height, "South");
  E->deltaPos(V3D{-width_d, 0, 3});
  auto S = make_square_bank(width, height, "East");
  S->deltaPos(V3D{0, -height_d, 3});
  auto W = make_square_bank(width, height, "West");
  E->deltaPos(V3D{width_d, 0, 3});

  auto l_curtain = make_square_bank(width, height, "Left curtain");
  l_curtain->deltaPos(V3D{-width_d, 0, 6});
  auto r_curtain = make_square_bank(width, height, "Right curtain");
  r_curtain->deltaPos(V3D{width_d, 0, 6});

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

  // Assemble flattened node node tree
  nodes[0].addChild(1);
  nodes[0].addChild(6);
  nodes[1].addChild(2);
  nodes[1].addChild(3);
  nodes[1].addChild(4);
  nodes[1].addChild(5);
  nodes[6].addChild(7);
  nodes[6].addChild(8);

  return nodes;
}
}
