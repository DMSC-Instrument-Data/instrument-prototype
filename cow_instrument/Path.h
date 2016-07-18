#ifndef PATH_H
#define PATH_H

#include "VectorOf.h"
#include "FixedLengthVector.h"

/**
 * Path is the per-detector Flight path through PathComponents.
 *
 * Stored indexes are PathComponent indexes in the InstrumentTree
 */
class Path : public VectorOf<Path> {
public:
  using VectorOf<Path>::VectorOf;
};

class Paths : public FixedLengthVector<Paths, Path> {
public:
  using FixedLengthVector<Paths, Path>::FixedLengthVector;
};

#endif
