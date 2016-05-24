#ifndef MASKFLAGS_H
#define MASKFLAGS_H

#include "Bool.h"
#include "FixedLengthVector.h"

/**
 * MaskFlags fixed length vector
 */
class MaskFlags : public FixedLengthVector<MaskFlags, Bool> {
  using FixedLengthVector<MaskFlags, Bool>::FixedLengthVector;
};

#endif
