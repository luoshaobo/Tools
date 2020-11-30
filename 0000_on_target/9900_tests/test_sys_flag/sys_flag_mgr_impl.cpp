#include "sys_flag_mgr_impl.hpp"

#define SYS_FLAG_MGR_NAME_PREFIX "_1"

static const char *SYS_FLAG_MGR_SHM_NAME = "_SysFlagMgr_Shm" SYS_FLAG_MGR_NAME_PREFIX;
static size_t SYS_FLAG_MGR_SHM_SIZE = 4096;
#ifdef WINDOWS
static const char *SYS_FLAG_MGR_FILE_LOCK_NAME = "D:\\sysflagmgr.lck";
#else
static const char *SYS_FLAG_MGR_FILE_LOCK_NAME = "/data/sysflagmgr.lck";
#endif // #ifdef WINDOWS
static const char *SYS_FLAG_MGR_COND_NAME = "_SysFlagMgr_ShmCond" SYS_FLAG_MGR_NAME_PREFIX;

const std::map<SysFlagMgrImpl::Value::Type, std::map<std::string, SysFlagMgrImpl::Value> > SysFlagMgrImpl::m_defaultItemValueTable = {
    {
        SysFlagMgrImpl::Value::Type::I32,
        {
#ifdef FOR_TEST
            { "AAA", { std::int32_t(-111) } },
#endif // #ifdef FOR_TEST
        }
    },
    {
        SysFlagMgrImpl::Value::Type::U32,
        {
#ifdef FOR_TEST
            { "AAA", { std::uint32_t(222) } },
#endif // #ifdef FOR_TEST
        }
    },
    {
        SysFlagMgrImpl::Value::Type::S16,
        {
            { "AAA", { SysFlagMgrImpl::Value::Type::S16, "S16" } },
        }
    },
    {
        SysFlagMgrImpl::Value::Type::S32,
        {
#ifdef FOR_TEST
            { "AAA", { SysFlagMgrImpl::Value::Type::S32, "S32" } },
#endif // #ifdef FOR_TEST
        }
    },
    {
        SysFlagMgrImpl::Value::Type::S64,
        {
#ifdef FOR_TEST
            { "AAA", { SysFlagMgrImpl::Value::Type::S64, "S64" } },
#endif // #ifdef FOR_TEST
        }
    },
};

const SysFlagMgrImpl::Value &SysFlagMgrImpl::GetDefaultItemValue(Value::Type type, const std::string &name)
{
    static const Value emptyValue;
    
    auto it1 = m_defaultItemValueTable.find(type);
    if (it1 != m_defaultItemValueTable.end()) {
        auto it2 = it1->second.find(name);
        if (it2 != it1->second.end()) {
            return it2->second;
        }
    }

    return emptyValue;
}

#ifdef FOR_TEST
static void TriggerCrashForTestIfNeeded()
{
    char *toCrash = getenv("TO_CRASH");
    if (toCrash != nullptr && std::string(toCrash) == "1") {
        printf("*** to crash!\n");
        *(int *)0 = 0xFFFFFFFF;
        printf("*** crashed!\n");
    }
}
#else
#define TriggerCrashForTestIfNeeded()
#endif // #ifdef FOR_TEST

SysFlagMgrImpl &SysFlagMgrImpl::GetInstance()
{
    static SysFlagMgrImpl sysFlagManager;
    return sysFlagManager;
}

SysFlagMgrImpl::EarlierInitializer::EarlierInitializer()
{
    FILE *pFile = NULL;

    pFile = fopen(SYS_FLAG_MGR_FILE_LOCK_NAME, "a+b");
    if (pFile != nullptr) {
        fclose(pFile);
    }
}

SysFlagMgrImpl::SysFlagMgrImpl()
    : SysFlagMgr()
    , m_earlierInitializer()
    , m_shm(boost::interprocess::open_or_create, SYS_FLAG_MGR_SHM_NAME, SYS_FLAG_MGR_SHM_SIZE)
    , m_shmIpcLock(SYS_FLAG_MGR_FILE_LOCK_NAME)
    , m_shmLocalLock()
    , m_shmCndMutex()
    , m_shmCnd(boost::interprocess::open_or_create, SYS_FLAG_MGR_COND_NAME)
    , m_itemValueMutex()
    , m_itemValueChangedCallbackInfoMap()
    , m_itemValueChangeWatchingThreadStopped(false)
    , m_itemValueChangeWatchingThread(&SysFlagMgrImpl::ItemValueChangeWatchingThreadProc, this)
{

}

SysFlagMgrImpl::~SysFlagMgrImpl()
{
    m_itemValueChangeWatchingThreadStopped = true;
    m_shmCnd.notify_all();
    m_itemValueChangeWatchingThread.join();
}

void SysFlagMgrImpl::SetItemValueChangedCallback(const std::string &name, Value::Type type, ItemValueChangedCallbackType callback)
{
    bool needToNotify = false;
    
    {
        std::unique_lock<std::recursive_mutex> lock(m_itemValueMutex);
        std::string formalName = GetFormalName(name, type);
        
        if (callback == nullptr) {
            auto it = m_itemValueChangedCallbackInfoMap.find(formalName);
            if (it != m_itemValueChangedCallbackInfoMap.end()) {
                m_itemValueChangedCallbackInfoMap.erase(it);
            }
        } else {
            m_itemValueChangedCallbackInfoMap[formalName] = ItemInfo{ name, type, Value{}, callback};
            needToNotify = true;
        }
    }
    
    if (needToNotify) {
        m_shmCnd.notify_all();
    }
}

SysFlagMgrImpl::Value SysFlagMgrImpl::GetItemValue(const std::string &name, Value::Type type)
{
    Value retValue;
    std::string formalName = GetFormalName(name, type);
    retValue.type = type;
    
    switch (type) {
        case Value::Type::I32:
            {
                std::int32_t *item = m_shm.find<std::int32_t>(formalName.c_str()).first;
                if (item == nullptr) {
                    retValue = GetDefaultItemValue(type, name);
                    return retValue;
                } else {
                    ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                    TriggerCrashForTestIfNeeded();
                    retValue.value.intValue.i32 = *item;
                    return retValue;
                }
            }
            break;
         case Value::Type::U32:
            {
                std::uint32_t *item = m_shm.find<std::uint32_t>(formalName.c_str()).first;
                if (item == nullptr) {
                    retValue = GetDefaultItemValue(type, name);
                    return retValue;
                } else {
                    ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                    TriggerCrashForTestIfNeeded();
                    retValue.value.intValue.u32 = *item;
                    return retValue;
                }
            }
            break;
        case Value::Type::S16:
            {
                ValueS16 *item = m_shm.find<ValueS16>(formalName.c_str()).first;
                if (item == nullptr) {
                    retValue = GetDefaultItemValue(type, name);
                    return retValue;
                } else {
                    ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                    TriggerCrashForTestIfNeeded();
                    retValue.value.strValue = std::string(item->s);
                    return retValue;
                }
            }
            break;
         case Value::Type::S32:
            {
                ValueS32 *item = m_shm.find<ValueS32>(formalName.c_str()).first;
                if (item == nullptr) {
                    retValue = GetDefaultItemValue(type, name);
                    return retValue;
                } else {
                    ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                    TriggerCrashForTestIfNeeded();
                    retValue.value.strValue = std::string(item->s);
                    return retValue;
                }
            }
            break;
        case Value::Type::S64:
            {
                ValueS64 *item = m_shm.find<ValueS64>(formalName.c_str()).first;
                if (item == nullptr) {
                    retValue = GetDefaultItemValue(type, name);
                    return retValue;
                } else {
                    ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                    TriggerCrashForTestIfNeeded();
                    retValue.value.strValue = std::string(item->s);
                    return retValue;
                }
            }
            break;
        default:
            {
            }
            break;
    }
    
    return retValue;
}

void SysFlagMgrImpl::SetItemValue(const std::string &name, const Value &value)
{
    std::string formalName = GetFormalName(name, value.type);

    switch (value.type) {
        case Value::Type::I32:
            {
                std::int32_t *item = m_shm.find_or_construct<std::int32_t>(formalName.c_str())();
                if (item == nullptr) {
                    return;
                } else {
                    {
                        ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                        TriggerCrashForTestIfNeeded();
                        *item = value.value.intValue.i32;
                    }
                    m_shmCnd.notify_all();
                }
            }
            break;
         case Value::Type::U32:
            {
                std::uint32_t *item = m_shm.find_or_construct<std::uint32_t>(formalName.c_str())();
                if (item == nullptr) {
                    return;
                } else {
                    {
                        ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                        TriggerCrashForTestIfNeeded();
                        *item = value.value.intValue.u32;
                    }
                    m_shmCnd.notify_all();
                }
            }
            break;
        case Value::Type::S16:
            {
                ValueS16 *item = m_shm.find_or_construct<ValueS16>(formalName.c_str())();
                if (item == nullptr) {
                    return;
                } else {
                    {
                        ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                        TriggerCrashForTestIfNeeded();
                        strncpy(item->s, value.value.strValue.c_str(), ValueS16::length);
                        item->s[ValueS16::length] = '\0';
                    }
                    m_shmCnd.notify_all();
                }
            }
            break;
        case Value::Type::S32:
            {
                ValueS32 *item = m_shm.find_or_construct<ValueS32>(formalName.c_str())();
                if (item == nullptr) {
                    return;
                } else {
                    {
                        ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                        TriggerCrashForTestIfNeeded();
                        strncpy(item->s, value.value.strValue.c_str(), ValueS32::length);
                        item->s[ValueS32::length] = '\0';
                    }
                    m_shmCnd.notify_all();
                }
            }
            break;
        case Value::Type::S64:
            {
                ValueS64 *item = m_shm.find_or_construct<ValueS64>(formalName.c_str())();
                if (item == nullptr) {
                    return;
                } else {
                    {
                        ShmScopeLock lock(m_shmLocalLock, m_shmIpcLock);
                        TriggerCrashForTestIfNeeded();
                        strncpy(item->s, value.value.strValue.c_str(), ValueS64::length);
                        item->s[ValueS64::length] = '\0';
                    }
                    m_shmCnd.notify_all();
                }
            }
            break;
        default:
            {
            }
            break;
    }
}

void SysFlagMgrImpl::ItemValueChangeWatchingThreadProc()
{
    while (!m_itemValueChangeWatchingThreadStopped) {
        {           
            boost::interprocess::scoped_lock<NullMutex> lock(m_shmCndMutex);
            m_shmCnd.wait(lock);            
        }
        
        if (m_itemValueChangeWatchingThreadStopped) {
            break;
        }
        
        {
            std::unique_lock<std::recursive_mutex> lock(m_itemValueMutex);

            for (auto &item: m_itemValueChangedCallbackInfoMap) {
                //const std::string &formalName = item.first;
                ItemInfo &itemInfo = item.second;
                if (itemInfo.callback != nullptr) {
                    Value newValue = GetItemValue(itemInfo.name, itemInfo.type);
                    Value &oldValue = itemInfo.value;
                    if (newValue != oldValue) {
                        itemInfo.value = newValue;
                        itemInfo.callback(itemInfo.name, itemInfo.value);
                    }
                }
            }
        }
    }    
}

std::string SysFlagMgrImpl::GetFormalName(const std::string &name, Value::Type type)
{
    std::string foramlName;

    switch (type) {
        case Value::Type::I32:
            {
                foramlName = std::string("_I32_") + name;
            }
            break;
         case Value::Type::U32:
            {
                foramlName = std::string("_U32_") + name;
            }
            break;
        case Value::Type::S16:
            {
                foramlName = std::string("_S16_") + name;
            }
            break;
        case Value::Type::S32:
            {
                foramlName = std::string("_S32_") + name;
            }
            break;
        case Value::Type::S64:
            {
                foramlName = std::string("_S64_") + name;
            }
            break;
        default:
            {
                foramlName = std::string("_UNKNOWN_") + name;
            }
            break;
    }

    return foramlName;
}
