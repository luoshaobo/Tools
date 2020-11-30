#ifndef SYS_FLAG_MGR_IMPL_H_7823749234823784782377632752378747238
#define SYS_FLAG_MGR_IMPL_H_7823749234823784782377632752378747238

#include <string>
#include <map>
#include <functional>
#include <thread>
#include <mutex>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/null_mutex.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include "sys_flag_mgr.hpp"

class SysFlagMgrImpl : public SysFlagMgr
{
private:
    class NullMutex : public boost::interprocess::null_mutex
    {
    public:
        class internal_mutex_type : public boost::interprocess::null_mutex
        {
        public:
            internal_mutex_type &internal_mutex() { return *this; }
        };

    public:
        NullMutex() : boost::interprocess::null_mutex(), m_internal_mutex() {}
        internal_mutex_type &internal_mutex() { return m_internal_mutex; }

    private:
        internal_mutex_type m_internal_mutex;
    };

    template <class Lock1, class Lock2>
    class ScopedLock2
    {
    public:
        ScopedLock2(Lock1& lock1, Lock2& lock2) : m_lock1(lock1), m_lock2(lock2)
        {
            m_lock1.lock();
            m_lock2.lock();
        }

        ~ScopedLock2()
        {
            m_lock2.unlock();
            m_lock1.unlock();
        }

    private:
        Lock1 &m_lock1;
        Lock2 &m_lock2;
    };
    typedef ScopedLock2<std::recursive_mutex, boost::interprocess::file_lock> ShmScopeLock;

    class EarlierInitializer
    {
    public:
        EarlierInitializer();
    };

private:
    struct ItemInfo {
        std::string name;
        Value::Type type;
        Value value;
        ItemValueChangedCallbackType callback;
    };

    struct ValueS16 {
        enum { length = 16 };
        char s[length + 1];
    };
    struct ValueS32 {
        enum { length = 32 };
        char s[length + 1];
    };
    struct ValueS64 {
        enum { length = 64 };
        char s[length + 1];
    };

private:
    SysFlagMgrImpl();
    
public:
    static SysFlagMgrImpl &GetInstance();

public:
    virtual ~SysFlagMgrImpl();
    
protected:
    virtual void SetItemValueChangedCallback(const std::string &name, Value::Type type, ItemValueChangedCallbackType callback) override;
    virtual Value GetItemValue(const std::string &name, Value::Type type) override;
    virtual void SetItemValue(const std::string &name, const Value &value) override;
    
private:
    void ItemValueChangeWatchingThreadProc();

private:
    std::string GetFormalName(const std::string &name, Value::Type type);
    
private:
    EarlierInitializer m_earlierInitializer;
    boost::interprocess::managed_shared_memory m_shm;
    boost::interprocess::file_lock m_shmIpcLock;                            // the lock to protect m_shm among processes
    std::recursive_mutex m_shmLocalLock;                                    // the lock to protect m_shm in the current process
    NullMutex m_shmCndMutex;
    boost::interprocess::named_condition m_shmCnd;
    
    std::recursive_mutex m_itemValueMutex;
    std::map<std::string, ItemInfo> m_itemValueChangedCallbackInfoMap;
    bool m_itemValueChangeWatchingThreadStopped;
    std::thread m_itemValueChangeWatchingThread;

private:
    static const std::map<Value::Type, std::map<std::string, Value> > m_defaultItemValueTable;
    static const Value &GetDefaultItemValue(Value::Type type, const std::string &name);
};

#endif // #ifndef SYS_FLAG_MGR_IMPL_H_7823749234823784782377632752378747238
