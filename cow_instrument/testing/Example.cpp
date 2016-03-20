#include "DetectorComponent.h"
#include "CompositeComponent.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "MoveCommand.h"
#include "Node.h"
#include "NullComponent.h"
#include "InstrumentTree.h"
#include <chrono>
#include <iostream>

using namespace testing;

namespace {

class ExampleTest : public ::testing::Test {

public:
  InstrumentTree m_instrument =
      InstrumentTree(Node_sptr(new Node(new NullComponent)));

private:
  CompositeComponent_sptr make_square_bank(size_t width, size_t height) {
    static size_t id = 1;
    CompositeComponent_sptr bank = std::make_shared<CompositeComponent>();
    for (size_t i = 0; i < width; ++i) {
      for (size_t j = 0; j < height; ++j) {
        bank->addComponent(std::make_shared<DetectorComponent>(
            id++, V3D{double(i), double(j), double(0)}));
      }
    }
    bank->deltaPos(
        V3D{-double(width) / 2, -double(height) / 2, 0}); // Center it
    return bank;
  }

protected:
  ExampleTest() {

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

    CompositeComponent_sptr N = make_square_bank(width, height);
    N->deltaPos(V3D{0, height_d, 3});
    CompositeComponent_sptr E = make_square_bank(width, height);
    E->deltaPos(V3D{-width_d, 0, 3});
    CompositeComponent_sptr S = make_square_bank(width, height);
    S->deltaPos(V3D{0, -height_d, 3});
    CompositeComponent_sptr W = make_square_bank(width, height);
    E->deltaPos(V3D{width_d, 0, 3});

    CompositeComponent_sptr l_curtain = make_square_bank(width, height);
    l_curtain->deltaPos(V3D{-width_d, 0, 6});
    CompositeComponent_sptr r_curtain = make_square_bank(width, height);
    r_curtain->deltaPos(V3D{width_d, 0, 6});

    Node_uptr root(new Node(CowPtr<Component>(new NullComponent)));
    Node_uptr front_trolley(new Node(CowPtr<Component>(new NullComponent)));
    Node_uptr nodeN(new Node(front_trolley.get(), CowPtr<Component>(N)));
    Node_uptr nodeE(new Node(front_trolley.get(), CowPtr<Component>(E)));
    Node_uptr nodeS(new Node(front_trolley.get(), CowPtr<Component>(S)));
    Node_uptr nodeW(new Node(front_trolley.get(), CowPtr<Component>(W)));
    Node_uptr rear_trolley(new Node(CowPtr<Component>(new NullComponent)));
    Node_uptr nodeLCurtain(new Node(rear_trolley.get(), CowPtr<Component>(l_curtain)));
    Node_uptr nodeRCurtain(new Node(rear_trolley.get(), CowPtr<Component>(r_curtain)));
    front_trolley->addChild(std::move(nodeN));
    front_trolley->addChild(std::move(nodeE));
    front_trolley->addChild(std::move(nodeS));
    front_trolley->addChild(std::move(nodeW));
    rear_trolley->addChild(std::move(nodeLCurtain));
    front_trolley->addChild(std::move(nodeRCurtain));
    root->addChild(std::move(front_trolley));
    root->addChild(std::move(rear_trolley));

    auto start = std::chrono::system_clock::now();

    m_instrument = InstrumentTree(std::move(root));

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "---------Instrument Tree Construction----------" << std::endl;
    std::cout << "Creation took " << elapsed_seconds.count() << "seconds" << std::endl;
    std::cout << "----------------------------------------" << std::endl;


  }

  virtual ~ExampleTest() {}
};

TEST_F(ExampleTest, simple_sans_example) {

  auto start = std::chrono::system_clock::now();

  size_t max = 100 * 100 * 6;
  double pos_x = 0;
  for (size_t i = 1; i < max; ++i) {
    const auto& det = m_instrument.getDetector(1);
    pos_x = det.getPos()[0];
  }

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "---------Single Access Metrics----------" << std::endl;
  std::cout << 100 * 100 * 6 << " accesses took " << elapsed_seconds.count() << "seconds" << std::endl;
  std::cout << "----------------------------------------" << std::endl;
}

} // namespace
