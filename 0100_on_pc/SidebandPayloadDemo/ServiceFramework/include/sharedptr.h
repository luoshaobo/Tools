#ifndef SHARED_PTR_H__
#define SHARED_PTR_H__

#include "atomiccounter.h"
#include "referencecounter.h"
#include "releasearraypolicy.h"
#include "releasepolicy.h"


template <class C, class RC = ReferenceCounter, class RP = ReleasePolicy<C> >
class SharedPtr

{
public:
  SharedPtr() : m_pCounter(new RC), m_ptr(0)
  {
  }

  SharedPtr(C* ptr) : m_pCounter(new RC), m_ptr(ptr)
  {
  }

  template <class Other, class OtherRP>
  SharedPtr(const SharedPtr<Other, RC, OtherRP>& ptr) : m_pCounter(ptr.m_pCounter), m_ptr(const_cast<Other*>(ptr.get()))//m_ptr(const_cast<Other*>(ptr.get()))
  {
    m_pCounter->duplicate();
  }

  SharedPtr(const SharedPtr& ptr) : m_pCounter(ptr.m_pCounter), m_ptr(ptr.m_ptr)
  {
    m_pCounter->duplicate();
  }

  ~SharedPtr()

  {
    release();
  }

  SharedPtr& assign(C* ptr)
  {
    if (get() != ptr) // damit m_ptr != ptr;
    {
      RC* pTmp = new RC;
      release();
      m_pCounter = pTmp;
      m_ptr = ptr;
    }
    return *this;
  }

  SharedPtr& assign(const SharedPtr& ptr)
  {
    if (&ptr != this)
    {
      SharedPtr tmp(ptr);
      swap(tmp);
    }
    return *this;
  }

  template <class Other, class OtherRP>
  SharedPtr& assign(const SharedPtr<Other, RC, OtherRP>& ptr)
  {
    if (ptr.get() != m_ptr)
    {
      SharedPtr tmp(ptr);
      swap(tmp);
    }
    return *this;
  }

  SharedPtr& operator = (C* ptr)
  {
    return assign(ptr);
  }

  SharedPtr& operator = (const SharedPtr& ptr)
  {
    return assign(ptr);
  }

  template <class Other, class OtherRP>
  SharedPtr& operator = (const SharedPtr<Other, RC, OtherRP>& ptr)
  {
    return assign<Other>(ptr);
  }

  void swap(SharedPtr& ptr)
  {
    std::swap(m_ptr, ptr.m_ptr);
    std::swap(m_pCounter, ptr.m_pCounter);
  }

  template <class Other>
  SharedPtr<Other, RC, RP> cast() const
  {
    Other* pOther = dynamic_cast<Other*>(m_ptr);
    if (pOther)
      return SharedPtr<Other, RC, RP>(m_pCounter, pOther);
    return SharedPtr<Other, RC, RP>();
  }

  template <class Other>
  SharedPtr<Other, RC, RP> unsafeCast() const
  {
    Other* pOther = static_cast<Other*>(m_ptr);
    return SharedPtr<Other, RC, RP>(m_pCounter, pOther);
  }

  C* operator -> ()
  {
    return deref();
  }

  const C* operator -> () const
  {
    return deref();
  }

  C& operator * ()
  {
    return *deref();
  }

  const C& operator * () const
  {
    return *deref();
  }

  C* get()
  {
    return m_ptr;
  }

  const C* get() const
  {
    return m_ptr;
  }

  operator C* ()
  {
    return m_ptr;
  }

  operator const C* () const
  {
    return m_ptr;
  }

  bool operator ! () const
  {
    return m_ptr == 0;
  }

  bool isNull() const
  {
    return m_ptr == 0;
  }

  bool operator == (const SharedPtr& ptr) const
  {
    return get() == ptr.get();
  }

  bool operator == (const C* ptr) const
  {
    return get() == ptr;
  }

  bool operator == (C* ptr) const
  {
    return get() == ptr;
  }

  bool operator != (const SharedPtr& ptr) const
  {
    return get() != ptr.get();
  }

  bool operator != (const C* ptr) const
  {
    return get() != ptr;
  }

  bool operator != (C* ptr) const
  {
    return get() != ptr;
  }

  bool operator < (const SharedPtr& ptr) const
  {
    return get() < ptr.get();
  }

  bool operator < (const C* ptr) const
  {
    return get() < ptr;
  }

  bool operator < (C* ptr) const
  {
    return get() < ptr;
  }

  bool operator <= (const SharedPtr& ptr) const
  {
    return get() <= ptr.get();
  }

  bool operator <= (const C* ptr) const
  {
    return get() <= ptr;
  }

  bool operator <= (C* ptr) const
  {
    return get() <= ptr;
  }

  bool operator > (const SharedPtr& ptr) const
  {
    return get() > ptr.get();
  }

  bool operator > (const C* ptr) const
  {
    return get() > ptr;
  }

  bool operator > (C* ptr) const
  {
    return get() > ptr;
  }

  bool operator >= (const SharedPtr& ptr) const
  {
    return get() >= ptr.get();
  }

  bool operator >= (const C* ptr) const
  {
    return get() >= ptr;
  }

  bool operator >= (C* ptr) const
  {
    return get() >= ptr;
  }

  int referenceCount() const
  {
    return m_pCounter->referenceCount();
  }

private:
  C* deref() const
  {
    /*if (!m_ptr)
      throw NullPointerException();*/

    return m_ptr;
  }

  void release()
  {
    int i = m_pCounter->release();
    if (i == 0)
    {
      RP::release(m_ptr);
      m_ptr = 0;

      delete m_pCounter;
      m_pCounter = 0;
    }
  }

  SharedPtr(RC* pCounter, C* ptr) : m_pCounter(pCounter), m_ptr(ptr)
    /// for cast operation
  {
    m_pCounter->duplicate();
  }

private:
  RC* m_pCounter;
  C*  m_ptr;

  template <class OtherC, class OtherRC, class OtherRP> friend class SharedPtr;
};


#endif //SHARED_PTR_H__