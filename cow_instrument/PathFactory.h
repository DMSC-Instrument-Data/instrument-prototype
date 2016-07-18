#ifndef PATHFACTORY_H
#define PATHFACTORY_H

#include "Path.h"

class InstrumentTree;

/**
 * FactoryMethod. Makes all flight paths for an InstrumentTree.
 *
 */
class PathFactory {
public:
  virtual Paths createL2(const InstrumentTree &instrument) const = 0;
  virtual Paths createL1(const InstrumentTree &instrument) const = 0;
  virtual ~PathFactory() {}
};

#endif
