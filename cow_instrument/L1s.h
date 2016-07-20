#ifndef L1S_H
#define L1S_H

#include "FixedLengthVector.h"

/**
 * L1 Values. Used to determine a cached L1.
 */
class L1s : public FixedLengthVector<L1s, double> {
  using FixedLengthVector<L1s, double>::FixedLengthVector;
};



#endif
