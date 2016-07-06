#ifndef MONITORFLAGS_H
#define MONITORFLAGS_H

#include "Bool.h"
#include "FixedLengthVector.h"

/**
 * Monitor flags fixed length vector
 */
class MonitorFlags : public FixedLengthVector<MonitorFlags, Bool> {
  using FixedLengthVector<MonitorFlags, Bool>::FixedLengthVector;
};

#endif
