#ifndef L2S_H
#define L2S_H

#include "FixedLengthVector.h"

/**
 * L2 Values. Used to determine a cached L2.
 */
class L2s : public FixedLengthVector<L2s, double> {
  using FixedLengthVector<L2s, double>::FixedLengthVector;
};



#endif
