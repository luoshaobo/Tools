#ifndef REF_COUNT_H
#define REF_COUNT_H

#include <set>

namespace TK_Tools {

template<typename T> class Ref;
template<typename T> class WeakRef;

class DummyLock
{
public:
    void Lock() {}
    void Unlock() {}
};

class WeakRefSharedBase
{
public:
    virtual ~WeakRefSharedBase() {}

    virtual void Lock() = 0;
    virtual void Unlock() = 0;
    virtual void SetObjectDeleted() = 0;
    virtual bool IsObjectDeleted() = 0;
    virtual void IncreaseRef_Locked() = 0;
    virtual void DecreaseRef_Locked() = 0;
    virtual int GetRefCount() = 0;
    virtual void * volatile GetObject() = 0;
};

template<typename LockClass>
class WeakRefShared : public WeakRefSharedBase
{
public:
    WeakRefShared(void *obj) : WeakRefSharedBase(), m_oLock(), m_nCount(0), m_pObject(obj) {}
    ~WeakRefShared() {}

    void Lock() {
        m_oLock.Lock();
    }

    void Unlock() {
        m_oLock.Unlock();
    }

    void SetObjectDeleted() {
        m_pObject = NULL;
    }

    bool IsObjectDeleted() {
        return (m_pObject == NULL);
    }

    void IncreaseRef_Locked() {
        m_oLock.Lock();
        m_nCount++;
        m_oLock.Unlock();
    }

    void DecreaseRef_Locked() {
        do {
            m_oLock.Lock();
            if (m_nCount > 0) {
                m_nCount--;
                if (m_nCount == 0 && m_pObject == NULL) {
                    delete this;
                    break;
                }
            }
            m_oLock.Unlock();
        } while (false);
    }

    int GetRefCount() {
        return m_nCount;
    }

    void * volatile GetObject() {
        return m_pObject;
    }

private:
    LockClass m_oLock;
    volatile int m_nCount;
    void * volatile m_pObject;
};

template<typename T, typename LockClass = DummyLock>
class Counted
{
protected:
    Counted() : m_oLock(), m_nCount(0), m_pWeakRefShared(NULL) {
    }

    virtual ~Counted() {
    }

public:
    T *Retain() {
        m_oLock.Lock();
        m_nCount++;
        m_oLock.Unlock();
        return static_cast<T *>(this);
    }

    void Release() {
        bool bToBeDeleted = false;

        do {
            m_oLock.Lock();

            if (m_pWeakRefShared != NULL) {
                m_pWeakRefShared->Lock();
            }
            
            if (m_nCount <= 0) {
                if (m_pWeakRefShared != NULL) {
                    m_pWeakRefShared->Unlock();
                }
                m_oLock.Unlock();
                break;
            }
            m_nCount--;
            if (m_nCount == 0) {
                bToBeDeleted = true;
            }

            if (bToBeDeleted) {
                if (m_pWeakRefShared != NULL) {
                    m_pWeakRefShared->SetObjectDeleted();
                    if (m_pWeakRefShared->GetRefCount() == 0) {
                        delete m_pWeakRefShared;
                        m_pWeakRefShared = NULL;
                    }
                }
                if (m_pWeakRefShared != NULL) {
                    m_pWeakRefShared->Unlock();
                }
                m_oLock.Unlock();
                delete this;
            } else {
                if (m_pWeakRefShared != NULL) {
                    m_pWeakRefShared->Unlock();
                }
                m_oLock.Unlock();
            }
        } while (false);
    }

    WeakRefShared<LockClass> * volatile GetWeakRefShared() {
        WeakRefShared<LockClass> *pWeakRefShared = NULL;

        m_oLock.Lock();
        if (m_pWeakRefShared == NULL) {
            m_pWeakRefShared = new WeakRefShared<LockClass>(reinterpret_cast<void *>(
                static_cast<T *>(this)  // convert from base class to derived class
            ));
        }
        pWeakRefShared = m_pWeakRefShared;
        m_oLock.Unlock();

        return pWeakRefShared;
    }

private:
    LockClass m_oLock;
    volatile int m_nCount;
    WeakRefShared<LockClass> * volatile m_pWeakRefShared;
};

template<typename T> 
class Ref
{
public:
    explicit Ref(T *pOther = NULL) : m_pObject(NULL) {
        Reset(pOther);
    }

    Ref(const Ref &rOther) : m_pObject(NULL) {
        Reset(rOther.m_pObject);
    }

    template<class Y>
    Ref(const Ref<Y> &rOther) : m_pObject(NULL) {
        Reset(rOther.m_pObject);
    }

    ~Ref() {
        if (m_pObject) {
            m_pObject->Release();
        }
    }

    void Reset(T *pOther = NULL) {
        if (pOther != NULL) {
            pOther->Retain();
        }
        if (m_pObject != NULL) {
            m_pObject->Release();
        }
        m_pObject = pOther;
    }

    Ref& operator=(const Ref &rOther) {
        Reset(rOther.m_pObject);
        return *this;
    }

    template<class Y>
    Ref& operator=(const Ref<Y> &rOther) {
        Reset(rOther.m_pObject);
        return *this;
    }

    Ref& operator=(T* pOther) {
        Reset(pOther);
        return *this;
    }

    template<class Y>
    Ref& operator=(Y* pOther) {
        Reset(pOther);
        return *this;
    }

    T& operator*() {
        return *m_pObject;
    }

    T* operator->() const {
        return m_pObject;
    }

    operator T*() const {
        return m_pObject;
    }

    bool operator==(const T* pOther) {
        return m_pObject == pOther;
    }

    bool operator==(const Ref &rOther) const {
        return m_pObject == rOther.m_pObject || *m_pObject == *(rOther.m_pObject);
    }

    template<class Y>
    bool operator==(const Ref<Y> &rOther) const {
        return m_pObject == rOther.m_pObject || *m_pObject == *(rOther.m_pObject);
    }

    bool operator!=(const T* pOther) {
        return !(*this == pOther);
    }

    bool IsNull() const {
        return m_pObject == NULL;
    }

private:
    T *m_pObject;
};

template<typename T>
class WeakRef
{
public:
    WeakRef() : m_pWeakRefShared(NULL) {
    
    }
    WeakRef(const WeakRef<T> &rOther) : m_pWeakRefShared(NULL) {
        Reset(rOther.m_pWeakRefShared);
    }
    WeakRef(const Ref<T> &rOther) : m_pWeakRefShared(NULL) {
        Reset((static_cast<T *>(rOther))->GetWeakRefShared());
    }

    ~WeakRef() {
        Reset(NULL);
    }

    WeakRef<T> &operator =(const WeakRef<T> &rOther) {
        Reset(rOther.m_pWeakRefShared);
        return *this;
    }
    WeakRef<T> &operator =(const Ref<T> &rOther) {
        Reset((static_cast<T *>(rOther))->GetWeakRefShared());
        return *this;
    }

    bool IsNull() {
        bool bIsNull = true;

        if (m_pWeakRefShared != NULL) {
            m_pWeakRefShared->Lock();
            bIsNull = m_pWeakRefShared->IsObjectDeleted();
            m_pWeakRefShared->Unlock();
        }

        return bIsNull;
    }

    Ref<T> GetRef() {
        Ref<T> ref(NULL);

        if (m_pWeakRefShared != NULL) {
            m_pWeakRefShared->Lock();
            if (!m_pWeakRefShared->IsObjectDeleted()) {
                ref = Ref<T>(reinterpret_cast<T *>(m_pWeakRefShared->GetObject()));
            }
            m_pWeakRefShared->Unlock();
        }

        return ref;
    }

    void Reset(WeakRefSharedBase *pWeakRefShared = NULL) {
        if (pWeakRefShared) {
            pWeakRefShared->IncreaseRef_Locked();
        }
        if (m_pWeakRefShared != 0) {
            m_pWeakRefShared->DecreaseRef_Locked();
        }
        m_pWeakRefShared = pWeakRefShared;
    }

private:
    WeakRefSharedBase *m_pWeakRefShared;
};

} // namespace TK_Tools {

#endif // #ifndef REF_COUNT_H
