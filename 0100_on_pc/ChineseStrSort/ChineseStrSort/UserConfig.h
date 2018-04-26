#ifndef USERCONFIG_H_873485993248734892875349560348693258934856348583458934895
#define USERCONFIG_H_873485993248734892875349560348693258934856348583458934895

#include "CSSGlobal.h"

namespace CSS {

enum {
    USERCONF_FULL_CODE_EQUAL_TO_HALF_CODE                       = 0x00000001,
    USERCONF_TRAD_CHAR_EQUAL_TO_SIMPLE_CODE                     = 0x00000002,
    USERCONF_SURNAME_FIRST                                      = 0x00000004,
    USERCONF_UNICODEPINYIN_LOADED_FROM_PLUGIN                   = 0x00000008,
    USERCONF_PHRASEPINYIN_LOADED_FROM_PLUGIN                    = 0x00000010,
};

class UserConfig
{
public:
    static UserConfig &GetInstance();
    ~UserConfig();

public:
    void SetFlag(unsigned int flag);
    void ClearFlag(unsigned int flag);
    bool IsFlagSet(unsigned int flag);

private:
    UserConfig();

private:
    unsigned int m_flags;
};

} // namespace CSS {

#endif // #ifndef USERCONFIG_H_873485993248734892875349560348693258934856348583458934895
