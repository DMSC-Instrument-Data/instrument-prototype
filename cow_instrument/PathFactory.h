#ifndef PATHFACTORY_H
#define PATHFACTORY_H

#include "Path.h"

class FlatTree;

/**
 * FactoryMethod. Makes all flight paths for an InstrumentTree.
 *
 */
template <typename InstTree> class PathFactory {
public:
  virtual Paths *createL2(const InstTree &instrument) const = 0;
  virtual Paths *createL1(const InstTree &instrument) const = 0;
  virtual ~PathFactory() {}
};

#endif
