#ifndef INT_TO_TYPE
#define INT_TO_TYPE

#include <memory>

/**
 This allows us to simply create unique types holding a T. I simply enforces
 that concrete types made from IntToType are incompatible.

 Typical usage:
        using MyUniqueDouble = IntToType<0, double>;
 */
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
  bool operator<(const IntToType<I, T> &other) const {
    return value < other.value;
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
  const T& const_ref(){
      return value;
  }
  IntToType<I, T> operator+(const T & increment){
      return IntToType<I,T>(value+increment);
  }

private:
  T value;
  enum { typeValue = I };
};

#endif
