#include "sys_flag_mgr.hpp"
#include "sys_flag_mgr_impl.hpp"

bool SysFlagMgr::Value::operator ==(const Value &rhs)
{
    bool same = false;
            
    if (type == rhs.type) {
        switch (type) {
            case Value::Type::I32:
                {
                    same = (value.intValue.i32 == rhs.value.intValue.i32);
                }
                break;
            case Value::Type::U32:
                {
                    same = (value.intValue.u32 == rhs.value.intValue.u32);
                }
                break;
            case Value::Type::S16:
            case Value::Type::S32:
            case Value::Type::S64:
                {
                    same = (value.strValue == rhs.value.strValue);
                }
                break;
            default:
                {
                    same = false;
                }
                break;
        }
    } else {
        same = false;
    }
            
    return same;
}
        
bool SysFlagMgr::Value::operator !=(const Value &rhs)
{
    return !operator ==(rhs);
}

SysFlagMgr::Value &SysFlagMgr::Value::operator =(const Value &rhs)
{
    type = rhs.type;
    value.intValue = rhs.value.intValue;
    value.strValue = rhs.value.strValue;

    return *this;
}

SysFlagMgr &SysFlagMgr::GetInstance()
{
    return SysFlagMgrImpl::GetInstance();
}

void SysFlagMgr::SetItemValueChangedCallback_I32(const std::string &name, ItemValueChangedCallbackType callback)
{
    SetItemValueChangedCallback(name, Value::Type::I32, callback);
}

std::int32_t SysFlagMgr::GetItemValue_I32(const std::string &name)
{
    Value value = GetItemValue(name, Value::Type::I32);
    return value.value.intValue.i32;
}

void SysFlagMgr::SetItemValue_I32(const std::string &name, std::int32_t value)
{
    Value value1;
    value1.type = Value::Type::I32;
    value1.value.intValue.i32 = value;

    SetItemValue(name, value1);
}

void SysFlagMgr::SetItemValueChangedCallback_U32(const std::string &name, ItemValueChangedCallbackType callback)
{
    SetItemValueChangedCallback(name, Value::Type::U32, callback);
}

std::uint32_t SysFlagMgr::GetItemValue_U32(const std::string &name)
{
    Value value = GetItemValue(name, Value::Type::U32);
    return value.value.intValue.u32;
}

void SysFlagMgr::SetItemValue_U32(const std::string &name, std::uint32_t value)
{
    Value value1;
    value1.type = Value::Type::U32;
    value1.value.intValue.u32 = value;

    SetItemValue(name, value1);
}

void SysFlagMgr::SetItemValueChangedCallback_S16(const std::string &name, ItemValueChangedCallbackType callback)
{
    SetItemValueChangedCallback(name, Value::Type::S16, callback);
}

std::string SysFlagMgr::GetItemValue_S16(const std::string &name)
{
    Value value = GetItemValue(name, Value::Type::S16);
    return value.value.strValue;
}

void SysFlagMgr::SetItemValue_S16(const std::string &name, const std::string &value)
{
    Value value1;
    value1.type = Value::Type::S16;
    value1.value.strValue = value;

    SetItemValue(name, value1);
}

void SysFlagMgr::SetItemValueChangedCallback_S32(const std::string &name, ItemValueChangedCallbackType callback)
{
    SetItemValueChangedCallback(name, Value::Type::S32, callback);
}

std::string SysFlagMgr::GetItemValue_S32(const std::string &name)
{
    Value value = GetItemValue(name, Value::Type::S32);
    return value.value.strValue;
}

void SysFlagMgr::SetItemValue_S32(const std::string &name, const std::string &value)
{
    Value value1;
    value1.type = Value::Type::S32;
    value1.value.strValue = value;

    SetItemValue(name, value1);
}

void SysFlagMgr::SetItemValueChangedCallback_S64(const std::string &name, ItemValueChangedCallbackType callback)
{
    SetItemValueChangedCallback(name, Value::Type::S64, callback);
}

std::string SysFlagMgr::GetItemValue_S64(const std::string &name)
{
    Value value = GetItemValue(name, Value::Type::S64);
    return value.value.strValue;
}

void SysFlagMgr::SetItemValue_S64(const std::string &name, const std::string &value)
{
    Value value1;
    value1.type = Value::Type::S64;
    value1.value.strValue = value;

    SetItemValue(name, value1);
}
