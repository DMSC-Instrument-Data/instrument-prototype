#ifndef FIXED_LENGTH_VECTOR_H
#define FIXED_LENGTH_VECTOR_H

#include <stdexcept>
#include <vector>

/**
 FixedLengthVector is a CRTP type.

 T is the derived class
 U is the type of the element to be stored in the vector
 */
template <class T, class U> class FixedLengthVector {
public:
  FixedLengthVector() = default;
  FixedLengthVector(size_t count, const U &value) : m_data(count, value) {}
  explicit FixedLengthVector(size_t count) : m_data(count) {}
  FixedLengthVector(std::initializer_list<U> init) : m_data(init) {}
  FixedLengthVector(const FixedLengthVector &) = default;
  FixedLengthVector(FixedLengthVector &&) = default;
  FixedLengthVector(const std::vector<U> &other) : m_data(other) {}
  FixedLengthVector(std::vector<U> &&other) : m_data(std::move(other)) {}
  template <class InputIt>
  FixedLengthVector(InputIt first, InputIt last)
      : m_data(first, last) {}

  FixedLengthVector &operator=(const FixedLengthVector &rhs) {
    checkAssignmentSize(rhs);
    m_data = rhs.m_data;
    return *this;
  }
  FixedLengthVector &operator=(FixedLengthVector &&rhs) {
    checkAssignmentSize(rhs);
    m_data = std::move(rhs.m_data);
    return *this;
  }
  FixedLengthVector &operator=(const std::vector<U> &rhs) {
    checkAssignmentSize(rhs);
    m_data = rhs;
    return *this;
  }
  FixedLengthVector &operator=(std::vector<U> &&rhs) {
    checkAssignmentSize(rhs);
    m_data = std::move(rhs);
    return *this;
  }
  FixedLengthVector &operator=(std::initializer_list<U> ilist) {
    checkAssignmentSize(ilist);
    m_data = ilist;
    return *this;
  }

  size_t size() const { return m_data.size(); }

  const U &operator[](size_t pos) const { return m_data[pos]; }
  U &operator[](size_t pos) { return m_data[pos]; }

  /// Returns a const reference to the underlying vector.
  const std::vector<U> &rawData() const { return m_data; }

protected:
  /** Returns a reference to the underlying vector.
   *
   * Note that this is not available in the public interface, since that would
   * allow for length modifications, which we need to prevent. */
  std::vector<U> &mutableRawData() { return m_data; }

  // This is used as base class only, cannot delete polymorphically, so
  // destructor is protected.
  ~FixedLengthVector() = default;

private:
  template <class Other> void checkAssignmentSize(const Other &other) {
    if (size() != other.size())
      throw std::logic_error("FixedLengthVector::operator=: size mismatch");
  }

  std::vector<U> m_data;

public:
  auto begin() -> decltype(m_data.begin()) { return m_data.begin(); }
  auto end() -> decltype(m_data.end()) { return m_data.end(); }
  auto begin() const -> decltype(m_data.begin()) { return m_data.begin(); }
  auto end() const -> decltype(m_data.end()) { return m_data.end(); }
  auto cbegin() const -> decltype(m_data.cbegin()) { return m_data.cbegin(); }
  auto cend() const -> decltype(m_data.cend()) { return m_data.cend(); }
  auto rbegin() -> decltype(m_data.rbegin()) { return m_data.rbegin(); }
  auto rend() -> decltype(m_data.rend()) { return m_data.rend(); }
  auto rbegin() const -> decltype(m_data.rbegin()) { return m_data.rbegin(); }
  auto rend() const -> decltype(m_data.rend()) { return m_data.rend(); }
  auto crbegin() const -> decltype(m_data.crbegin()) { return m_data.crbegin(); }
  auto crend() const -> decltype(m_data.crend()) { return m_data.crend(); }
};

#endif
