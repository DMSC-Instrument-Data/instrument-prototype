#ifndef INT_TO_TYPE
#define INT_TO_TYPE

#include <utility>
/**
 This allows us to simply create unique types holding a T. I simply enforces
 that concrete types made from IntToType are incompatible.

 Typical usage:
        using MyUniqueDouble = IntToType<0, double>;
 */
template <int I, typename T> class IntToType {
public:
  using StorageType = T;
  explicit IntToType(const T &val) : value(val) {}
  explicit IntToType(T &&val) : value(std::move(val)) {}
  IntToType(const IntToType<I, T> &other) : value(other.value) {}
  IntToType(IntToType<I, T> &&other) : value(std::move(other.value)) {}
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
  const T &const_ref() const { return value; }
  IntToType<I, T> operator+(const T & increment){
      return IntToType<I,T>(value+increment);
  }

  T value;

private:
  enum { typeValue = I };
};

#endif
