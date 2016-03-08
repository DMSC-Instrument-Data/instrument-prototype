#ifndef COWPTR_H
#define COWPTR_H

#include <memory>

template <class T> class CowPtr {
public:
  typedef std::shared_ptr<T> RefPtr;

private:
  RefPtr m_sp;

public:

  void copy() {
    T *tmp = m_sp.get();
    if (!(tmp == 0 || m_sp.unique())) {
      m_sp = RefPtr(tmp->clone());
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
  const T& const_ref(){
      return *m_sp;
  }

};

#endif
