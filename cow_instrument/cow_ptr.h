#ifndef COWPTR_H
#define COWPTR_H

#include <memory>
#include <type_traits>

template <class T> class CowPtr {
public:
  typedef std::shared_ptr<T> RefPtr;

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
  void copy() {
    T *tmp = m_sp.get();
    if (!(tmp == 0 || m_sp.unique())) {
      doCopy(tmp);
    }
  }

  CowPtr(T *t) : m_sp(t) {}
  CowPtr(const RefPtr &refptr) : m_sp(refptr) {}
  const T &operator*() const { return *m_sp; }
  T &operator*() {
    copy();
    return *m_sp;
  }
  const T *operator->() const { return m_sp.operator->(); }
  T *operator->() {
    copy();
    return m_sp.operator->();
  }

  // CowPtr &operator=(const CowPtr<T> &) = default;

  const T &const_ref() const { return *m_sp; }
};

#endif
