#ifndef INT_TO_TYPE
#define INT_TO_TYPE

#include <memory>

template <int I, typename T> class IntToType {
public:
  explicit IntToType(const T &val) : value(val) {}
  explicit IntToType(T &&val) : value(val) {}
  IntToType(const IntToType<I, T> &other) : value(other.value) {}
  IntToType<I, T> &operator=(const IntToType<I, T> &other) {
    value = other.value;
    return *this;
  }
  bool operator==(const IntToType<I, T> &other) const {
    return value == other.value;
  }
  IntToType<I, T>& operator++()
  {
       ++value;
      return this;
  }
  IntToType<I, T> operator++(int)
  {
      IntToType<I,T> temp(value);
      ++value;
      return temp;
  }

private:
  T value;
  enum { typeValue = I };
};

#endif
