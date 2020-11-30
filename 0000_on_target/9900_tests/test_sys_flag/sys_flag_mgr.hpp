#ifndef SYS_FLAG_MGR_H_7634782947238376743657489845894868995
#define SYS_FLAG_MGR_H_7634782947238376743657489845894868995

#include <stdint.h>
#include <string>
#include <functional>

class SysFlagMgr
{
public:
    struct Value {
        enum class Type {
            I32 = 0,           // singend int, 32 bits
            U32,               // unsigned int, 32 bits
            S16,               // string, 16 bytes as maximum
            S32,               // string, 32 bytes as maximum
            S64,               // string, 64 bytes as maximum
        };

        Type type;
        struct {
            union {
                std::int32_t i32;
                std::uint32_t u32;
            } intValue;
            std::string strValue;
        } value;
        
        Value() : type(Type::I32), value() {}
        Value(std::int32_t a_value) : type(Type::I32), value() { value.intValue.i32 = a_value; }
        Value(std::uint32_t a_value) : type(Type::U32), value() { value.intValue.u32 = a_value; }
        Value(Type a_type, const std::string &a_value) : type(a_type), value() { value.strValue = a_value; }
        bool operator ==(const Value &rhs);
        bool operator !=(const Value &rhs);
        Value &operator =(const Value &rhs);
    };
    
    typedef std::function<void (const std::string &name, const Value &value)> ItemValueChangedCallbackType;
    
public:
    static SysFlagMgr &GetInstance();
    
public:
    void SetItemValueChangedCallback_I32(const std::string &name, ItemValueChangedCallbackType callback);
    std::int32_t GetItemValue_I32(const std::string &name);
    void SetItemValue_I32(const std::string &name, std::int32_t value);

    void SetItemValueChangedCallback_U32(const std::string &name, ItemValueChangedCallbackType callback);
    std::uint32_t GetItemValue_U32(const std::string &name);
    void SetItemValue_U32(const std::string &name, std::uint32_t value);

    void SetItemValueChangedCallback_S16(const std::string &name, ItemValueChangedCallbackType callback);
    std::string GetItemValue_S16(const std::string &name);
    void SetItemValue_S16(const std::string &name, const std::string &value);

    void SetItemValueChangedCallback_S32(const std::string &name, ItemValueChangedCallbackType callback);
    std::string GetItemValue_S32(const std::string &name);
    void SetItemValue_S32(const std::string &name, const std::string &value);

    void SetItemValueChangedCallback_S64(const std::string &name, ItemValueChangedCallbackType callback);
    std::string GetItemValue_S64(const std::string &name);
    void SetItemValue_S64(const std::string &name, const std::string &value);
    
public:
    virtual ~SysFlagMgr() {}

protected:
    virtual void SetItemValueChangedCallback(const std::string &name, Value::Type type, ItemValueChangedCallbackType callback) = 0;
    virtual Value GetItemValue(const std::string &name, Value::Type type) = 0;
    virtual void SetItemValue(const std::string &name, const Value &value) = 0;
};

#endif // #ifndef SYS_FLAG_MGR_H_7634782947238376743657489845894868995
