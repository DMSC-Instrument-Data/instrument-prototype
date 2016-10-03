#ifndef COWPTR_H
#define COWPTR_H

#include <memory>
#include <type_traits>

template <class T> class CowPtr {
public:
  typedef std::shared_ptr<T> RefPtr;
  typedef std::shared_ptr<const T> RefConstPtr;

private:
  RefPtr m_sp;

  /**
   * Prefer to use Clone if that's available so that COW ptrs
   * can be built around base class types, but support construction
   * via derived types.
   */
  template <typename U>
  typename std::enable_if<
      std::is_member_function_pointer<decltype(&U::clone)>::value, void>::type
  doCopy(U *arg) {
    m_sp = RefPtr(arg->clone());
  }

  /**
   * Fall back to do copy via a copy constructor if the type does
   * not support clone.
   */
  void doCopy(...) {
    T *arg = m_sp.get();
    m_sp = RefPtr(new typename RefPtr::element_type(*arg));
  }

public:
  void copy();
  CowPtr(T *t);
  CowPtr(const RefPtr &refptr);
  const T &operator*() const;
  T &operator*();
  const T *operator->() const;
  T *operator->();
  const T &const_ref() const;
  bool operator==(const CowPtr<T> &other) const;
  bool unique() const;
  bool copyable() const;
  RefPtr heldValue() const;
};

template <typename T> void CowPtr<T>::copy() {
  T *tmp = m_sp.get();
  if (!(tmp == 0 || m_sp.unique())) {
    doCopy(tmp);
  }
}

template <typename T> CowPtr<T>::CowPtr(T *t) : m_sp(t) {}

template <typename T> CowPtr<T>::CowPtr(const RefPtr &refptr) : m_sp(refptr) {}

template <typename T> const T &CowPtr<T>::operator*() const { return *m_sp; }

template <typename T> T &CowPtr<T>::operator*() {
  copy();
  return *m_sp;
}

template <typename T> const T *CowPtr<T>::operator->() const {
  return m_sp.operator->();
}

template <typename T> T *CowPtr<T>::operator->() {
  copy();
  return m_sp.operator->();
}

template <typename T> const T &CowPtr<T>::const_ref() const { return *m_sp; }

template <typename T> bool CowPtr<T>::operator==(const CowPtr<T> &other) const {
  return m_sp.get() == &other.const_ref();
}

template <typename T> bool CowPtr<T>::unique() const { return m_sp.unique(); }

template <typename T> bool CowPtr<T>::copyable() const { return !unique(); }

template <typename T> typename CowPtr<T>::RefPtr CowPtr<T>::heldValue() const {
  return m_sp;
}
#endif
