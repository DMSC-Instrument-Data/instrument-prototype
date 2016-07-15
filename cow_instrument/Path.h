#ifndef PATH_H
#define PATH_H

#include "VectorOf.h"
#include "FixedLengthVector.h"

class Path : public VectorOf<Path> {
public:
  using VectorOf<Path>::VectorOf;
};
#endif
