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
  VectorOf(size_t index);
  VectorOf(std::initializer_list<size_t> init);
  VectorOf(const VectorOf<T> &) = default;
  VectorOf(VectorOf<T> &&) = default;
  VectorOf(const std::vector<size_t> &detectorIndexes);
  VectorOf(std::vector<size_t> &&detectorIndexes);

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
  std::vector<size_t> m_detectorIndexes;
};

template <typename T>
VectorOf<T>::VectorOf(size_t detectorIndex)
    : m_detectorIndexes(1, detectorIndex) {}
template <typename T>
VectorOf<T>::VectorOf(std::initializer_list<size_t> init)
    : m_detectorIndexes(init) {}
template <typename T>
VectorOf<T>::VectorOf(const std::vector<size_t> &detectorIndexes)
    : m_detectorIndexes(detectorIndexes) {}
template <typename T>
VectorOf<T>::VectorOf(std::vector<size_t> &&detectorIndexes)
    : m_detectorIndexes(std::move(detectorIndexes)) {}
template <typename T>
VectorOf<T> &VectorOf<T>::operator=(const std::vector<size_t> &rhs) {
  m_detectorIndexes = rhs;
  return *this;
}
template <typename T>
VectorOf<T> &VectorOf<T>::operator=(std::vector<size_t> &&rhs) {
  m_detectorIndexes = std::move(rhs);
  return *this;
}
template <typename T>
VectorOf<T> &VectorOf<T>::operator=(std::initializer_list<size_t> ilist) {
  m_detectorIndexes = ilist;
  return *this;
}
template <typename T>
bool VectorOf<T>::operator==(const VectorOf<T> &other) const {
  return m_detectorIndexes == other.m_detectorIndexes;
}
template <typename T>
bool VectorOf<T>::operator!=(const VectorOf<T> &other) const {
  return m_detectorIndexes != other.m_detectorIndexes;
}
template <typename T> const std::vector<size_t> &VectorOf<T>::indexes() const {
  return m_detectorIndexes;
}
template <typename T> size_t VectorOf<T>::size() const {
  return m_detectorIndexes.size();
}
template <typename T> const size_t &VectorOf<T>::operator[](size_t pos) const {
  return m_detectorIndexes[pos];
}
template <typename T> size_t &VectorOf<T>::operator[](size_t pos) {
  return m_detectorIndexes[pos];
}

#endif
