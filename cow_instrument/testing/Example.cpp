#include "Detector.h"
#include "CompositeComponent.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "MoveCommand.h"
#include "Node.h"
#include "NullComponent.h"
#include "InstrumentTree.h"

using namespace testing;

namespace {


CompositeComponent_sptr make_square_bank(size_t width, size_t height){
    CompositeComponent_sptr bank = std::make_shared<CompositeComponent>();
    for(size_t i = 0; i < width; ++i){
        for(size_t j = 0; j < height; ++j){
            bank->addComponent(std::make_shared<Detector>(i * 100 + j, V3D{double(i), double(j), double(0)} ));
        }
    }
    bank->deltaPos(V3D{-double(width)/2, -double(height)/2, 0}); // Center it
    return bank;
}


TEST(example, simple_sans_example){
    
    
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


    Node_sptr root = std::make_shared<Node>(CowPtr<Component>(new NullComponent));
    Node_sptr front_trolley = std::make_shared<Node>(root, CowPtr<Component>(new NullComponent));
    Node_sptr nodeN = std::make_shared<Node>(front_trolley, CowPtr<Component>(N));
    Node_sptr nodeE = std::make_shared<Node>(front_trolley, CowPtr<Component>(E));
    Node_sptr nodeS = std::make_shared<Node>(front_trolley, CowPtr<Component>(S));
    Node_sptr nodeW = std::make_shared<Node>(front_trolley, CowPtr<Component>(W));
    Node_sptr rear_trolley = std::make_shared<Node>(root, CowPtr<Component>(new NullComponent));
    Node_sptr nodeLCurtain = std::make_shared<Node>(rear_trolley, CowPtr<Component>(l_curtain));
    Node_sptr nodeRCurtain = std::make_shared<Node>(rear_trolley, CowPtr<Component>(r_curtain));
    root->addChild(front_trolley);
    root->addChild(rear_trolley);
    front_trolley->addChild(nodeN);
    front_trolley->addChild(nodeE);
    front_trolley->addChild(nodeS);
    front_trolley->addChild(nodeW);
    rear_trolley->addChild(nodeLCurtain);
    front_trolley->addChild(nodeRCurtain);

    InstrumentTree tree(root);
    
    
}






} // namespace
