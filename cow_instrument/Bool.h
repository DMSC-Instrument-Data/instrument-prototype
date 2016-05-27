#ifndef BOOL_H
#define BOOL_H

/**
 * Boolean type to prevent boolean-bit specialization when using
 * std::vector
 */
struct Bool {
public:
  Bool(bool value) : m_value(value) {}
  operator bool() const { return m_value; }

private:
  bool m_value;
};

#endif
