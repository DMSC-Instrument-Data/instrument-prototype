#include "CompositeComponent.h"
#include "cow_ptr.h"
#include "DetectorComponent.h"
#include "DetectorComponentFactory.h"
#include "InstrumentTree.h"
#include "MoveCommand.h"
#include "Node.h"
#include "NullComponent.h"

#include "benchmark/benchmark_api.h"
#include <chrono>
#include <iostream>

namespace {

std::shared_ptr<CompositeComponent>
make_square_bank(size_t width, size_t height,
                 const DetectorComponentFactory &detectorFactory) {
  static DetectorIdType detectorId(1);
  static ComponentIdType componentId(1);
  auto bank = std::make_shared<CompositeComponent>(ComponentIdType(0));

  for (size_t i = 0; i < width; ++i) {
    for (size_t j = 0; j < height; ++j) {
      bank->addComponent(detectorFactory.create_unique(
          componentId++, detectorId++, V3D{double(i), double(j), double(0)}));
    }
  }
  bank->deltaPos(V3D{-double(width) / 2, -double(height) / 2, 0}); // Center it
  return bank;
}

Node_uptr construct_root_node() {
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

  DetectorComponentFactory detectorFactory;
  auto N = make_square_bank(width, height, detectorFactory);
  N->deltaPos(V3D{0, height_d, 3});
  auto E = make_square_bank(width, height, detectorFactory);
  E->deltaPos(V3D{-width_d, 0, 3});
  auto S = make_square_bank(width, height, detectorFactory);
  S->deltaPos(V3D{0, -height_d, 3});
  auto W = make_square_bank(width, height, detectorFactory);
  E->deltaPos(V3D{width_d, 0, 3});

  auto l_curtain = make_square_bank(width, height, detectorFactory);
  l_curtain->deltaPos(V3D{-width_d, 0, 6});
  auto r_curtain = make_square_bank(width, height, detectorFactory);
  r_curtain->deltaPos(V3D{width_d, 0, 6});

  Node_uptr root(new Node(CowPtr<Component>(new NullComponent)));
  Node_uptr front_trolley(new Node(CowPtr<Component>(new NullComponent)));
  Node_uptr nodeN(new Node(front_trolley.get(), CowPtr<Component>(N)));
  Node_uptr nodeE(new Node(front_trolley.get(), CowPtr<Component>(E)));
  Node_uptr nodeS(new Node(front_trolley.get(), CowPtr<Component>(S)));
  Node_uptr nodeW(new Node(front_trolley.get(), CowPtr<Component>(W)));
  Node_uptr rear_trolley(new Node(CowPtr<Component>(new NullComponent)));
  Node_uptr nodeLCurtain(
      new Node(rear_trolley.get(), CowPtr<Component>(l_curtain)));
  Node_uptr nodeRCurtain(
      new Node(rear_trolley.get(), CowPtr<Component>(r_curtain)));
  front_trolley->addChild(std::move(nodeN));
  front_trolley->addChild(std::move(nodeE));
  front_trolley->addChild(std::move(nodeS));
  front_trolley->addChild(std::move(nodeW));
  rear_trolley->addChild(std::move(nodeLCurtain));
  front_trolley->addChild(std::move(nodeRCurtain));
  root->addChild(std::move(front_trolley));
  root->addChild(std::move(rear_trolley));

  return root;
}



void BM_InstrumentTreeConstruction(benchmark::State& state) {
  while (state.KeepRunning()) {
    state.PauseTiming();

    InstrumentTree instrument(Node_uptr(new Node(new NullComponent)), 0);
    Node_uptr root = construct_root_node();

    state.ResumeTiming();

    instrument = InstrumentTree(std::move(root), 60000);
  }
}
BENCHMARK(BM_InstrumentTreeConstruction);

void BM_InstrumentTreeConstruction2(benchmark::State& state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    Node_uptr root = construct_root_node();
    state.ResumeTiming();

    InstrumentTree instrument(std::move(root), 60000);
  }
}
BENCHMARK(BM_InstrumentTreeConstruction2);

void BM_SingleAccessMetrics(benchmark::State& state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    InstrumentTree instrument(construct_root_node(), 60000);
    state.ResumeTiming();

    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = instrument.getDetector(i);
      pos_x = det.getPos()[0];
    }
  }
}
BENCHMARK(BM_SingleAccessMetrics);

void BM_SingleAccessMetricsStatic(benchmark::State& state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    static InstrumentTree instrument(construct_root_node(), 60000);
    state.ResumeTiming();

    size_t max = 100 * 100 * 6;
    double pos_x = 0;
    for (size_t i = 1; i < max; ++i) {
      const auto &det = instrument.getDetector(i);
      pos_x = det.getPos()[0];
    }
  }
}
BENCHMARK(BM_SingleAccessMetricsStatic);

} // namespace

BENCHMARK_MAIN()
