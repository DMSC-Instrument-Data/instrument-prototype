#ifndef STANDARD_INSTRUMENT_H
#define STANDARD_INSTRUMENT_H

#include <memory>
class Node;
namespace std_instrument {
std::unique_ptr<Node> construct_root_node();
}

#endif
