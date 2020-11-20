#ifndef DELEGATE_H__
#define DELEGATE_H__

#include "idelegate.h"
#include "mutex.h"
#include "scopedlock.h"

template <class TObj, class TArgs>
class Lib_API Delegate : public IDelegate<TArgs>
{
public:
  typedef void (TObj::*NotifyMethod)(TArgs&);

  Delegate(TObj* obj, NotifyMethod method) :
    m_delegateObject(obj),
    m_delegateMethod(method)
  {
  }

  Delegate(const Delegate& rhs) :
    IDelegate<TArgs>(rhs),
    m_delegateObject(rhs.m_delegateObject),
    m_delegateMethod(rhs.m_delegateMethod)
  {
  }

  virtual ~Delegate()
  {
  }

  Delegate& operator = (const Delegate& rhs)
  {
    if (&rhs != this)
    {
      this->m_delegateObject = rhs.m_delegateObject;
      this->m_delegateMethod = rhs.m_delegateMethod;
    }
    return *this;
  }

  bool notify(const void* sender, TArgs& arguments)
  {
    ScopedLock lock(m_mutex);
    bool bRet = false;

    if (m_delegateObject)
    {
      (m_delegateObject->*m_delegateMethod)(arguments);
      bRet = true;
    }

    return bRet;
  }

  bool equals(const IDelegate<TArgs>& rhs) const
  {
    const Delegate* pRhsDelegate = reinterpret_cast<const Delegate*>(rhs.unwrap());

    return (
      pRhsDelegate &&
      m_delegateObject == pRhsDelegate->m_delegateObject &&
      m_delegateMethod == pRhsDelegate->m_delegateMethod);
  }

  IDelegate<TArgs>* clone() const
  {
    return new Delegate(*this);
  }

  void disable()
  {
    ScopedLock lock(m_mutex);
    m_delegateObject = 0;
  }

protected:
  TObj*        m_delegateObject;
  NotifyMethod m_delegateMethod;
  Mutex        m_mutex;

private:
  Delegate();
};

#endif // DELEGATE_H__
