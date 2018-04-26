#include "UserConfig.h"

namespace CSS {

UserConfig &UserConfig::GetInstance()
{
    static UserConfig userConfig;
    return userConfig;
}

UserConfig::UserConfig()
    : m_flags(0)
{
    SetFlag(USERCONF_FULL_CODE_EQUAL_TO_HALF_CODE);
    SetFlag(USERCONF_TRAD_CHAR_EQUAL_TO_SIMPLE_CODE);
}

UserConfig::~UserConfig()
{

}

void UserConfig::SetFlag(unsigned int flag)
{
    m_flags |= flag;
}

void UserConfig::ClearFlag(unsigned int flag)
{
    m_flags &= ~flag;
}

bool UserConfig::IsFlagSet(unsigned int flag)
{
    return (m_flags & flag) == flag;
}

} // namespace CSS {
