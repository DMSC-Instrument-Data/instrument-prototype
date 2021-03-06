#ifndef VECTOROF_H
#define VECTOROF_H
#include <vector>

/**
  CRTP class where T is the type to create the VectorOf type around.

  VectorOf<T> uses a vector<size_t>, the vector itself does not take the type
  parameter, the T type argument is used to ensure type incompatibility.
 */

template <typename T> class VectorOf {
public:
  VectorOf(size_t size);
  VectorOf(size_t size, const size_t &value);
  VectorOf(std::initializer_list<size_t> init);
  VectorOf(const VectorOf<T> &) = default;
  VectorOf(VectorOf<T> &&) = default;
  VectorOf(const std::vector<size_t> &indexes);
  VectorOf(std::vector<size_t> &&indexes);

  VectorOf &operator=(const VectorOf<T> &rhs) = default;
  VectorOf &operator=(VectorOf<T> &&rhs) = default;
  VectorOf &operator=(const std::vector<size_t> &rhs);
  VectorOf &operator=(std::vector<size_t> &&rhs);
  VectorOf &operator=(std::initializer_list<size_t> ilist);
  bool operator==(const VectorOf<T> &other) const;
  bool operator!=(const VectorOf<T> &other) const;
  const std::vector<size_t> &indexes() const;

  size_t size() const;
  const size_t &operator[](size_t pos) const;
  size_t &operator[](size_t pos);

private:
  std::vector<size_t> m_indexes;
};

template <typename T> VectorOf<T>::VectorOf(size_t size) : m_indexes(size, 0) {}
template <typename T>
VectorOf<T>::VectorOf(size_t size, const size_t &value)
    : m_indexes(size, value) {}
template <typename T>
VectorOf<T>::VectorOf(std::initializer_list<size_t> init)
    : m_indexes(init) {}
template <typename T>
VectorOf<T>::VectorOf(const std::vector<size_t> &indexes)
    : m_indexes(indexes) {}
template <typename T>
VectorOf<T>::VectorOf(std::vector<size_t> &&indexes)
    : m_indexes(std::move(indexes)) {}
template <typename T>
VectorOf<T> &VectorOf<T>::operator=(const std::vector<size_t> &rhs) {
  m_indexes = rhs;
  return *this;
}
template <typename T>
VectorOf<T> &VectorOf<T>::operator=(std::vector<size_t> &&rhs) {
  m_indexes = std::move(rhs);
  return *this;
}
template <typename T>
VectorOf<T> &VectorOf<T>::operator=(std::initializer_list<size_t> ilist) {
  m_indexes = ilist;
  return *this;
}
template <typename T>
bool VectorOf<T>::operator==(const VectorOf<T> &other) const {
  return m_indexes == other.m_indexes;
}
template <typename T>
bool VectorOf<T>::operator!=(const VectorOf<T> &other) const {
  return m_indexes != other.m_indexes;
}
template <typename T> const std::vector<size_t> &VectorOf<T>::indexes() const {
  return m_indexes;
}
template <typename T> size_t VectorOf<T>::size() const {
  return m_indexes.size();
}
template <typename T> const size_t &VectorOf<T>::operator[](size_t pos) const {
  return m_indexes[pos];
}
template <typename T> size_t &VectorOf<T>::operator[](size_t pos) {
  return m_indexes[pos];
}

#endif
