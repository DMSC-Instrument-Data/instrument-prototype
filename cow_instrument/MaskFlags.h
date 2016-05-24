#ifndef MASKFLAGS_H
#define MASKFLAGS_H

#include <FixedLengthVector.h>

/**
 * Boolean type to prevent boolean-bit specialisation when using
 * std::vector
 */
struct Bool {
public:
  Bool(bool value) : m_value(value) {}
  operator bool() const { return m_value;}
private:
  bool m_value;
};

/**
 * MaskFlags fixed length vector
 */
class MaskFlags : public FixedLengthVector<MaskFlags, Bool> {

  using FixedLengthVector<MaskFlags, Bool>::FixedLengthVector;
};

#endif
