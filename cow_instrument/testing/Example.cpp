#include "Detector.h"
#include "CompositeComponent.h"
#include "cow_ptr.h"
#include "gtest/gtest.h"
#include "MoveCommand.h"
#include "Node.h"

using namespace testing;

namespace {


CompositeComponent_sptr make_square_bank(){
    CompositeComponent_sptr bank = std::make_shared<CompositeComponent>();
    for(size_t i = 0; i < 100; ++i){
        for(size_t j = 0; j < 100; ++j){
            bank->addComponent(std::make_shared<Detector>(i * 100 + j, V3D{double(i), double(j), double(0)} ));
        }
    }
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

    CompositeComponent_sptr N = make_square_bank();
    CompositeComponent_sptr E = make_square_bank();
    CompositeComponent_sptr S = make_square_bank();
    CompositeComponent_sptr W = make_square_bank();
    
    
}






} // namespace
