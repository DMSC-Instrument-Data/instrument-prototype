#ifndef L2S_H
#define L2S_H

#include "Bool.h"
#include "FixedLengthVector.h"

/**
 * L2 Set Type. Used to determine if the L2 has been set.
 */
class L2Flags : public FixedLengthVector<L2Flags, Bool> {
  using FixedLengthVector<L2Flags, Bool>::FixedLengthVector;
};

/**
 * L2 Values. Used to determine a cached L2.
 */
class L2s : public FixedLengthVector<L2s, double> {
  using FixedLengthVector<L2s, double>::FixedLengthVector;
};



#endif
