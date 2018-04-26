#ifndef CHSTRPARSER_H_782384230059046783587278472747889340696897956807680989689
#define CHSTRPARSER_H_782384230059046783587278472747889340696897956807680989689

#include "CSSGlobal.h"

namespace CSS {

enum CharCata {
    EN_CC_SYMBOL = 0,
    EN_CC_DIGTAL,
    EN_CC_LETTER,
    EN_CC_HANZI,
    EN_CC_OTHER, 
};

enum ChCharTone {
    EN_CCT_1 = 1,
    EN_CCT_2,
    EN_CCT_3,
    EN_CCT_4,
    EN_CCT_NONE,
};

enum ChCharAttrib {
    EN_CCA_UPCASE                 = 0x00000001,
    EN_CCA_FULLCODE               = 0x00000002,
    EN_CCA_TRADCH                 = 0x00000004,
};

struct ChChar {
    ChChar() 
        : cata(EN_CC_HANZI)
        , code(0)
        , pinyinWithoutTone()
        , tone(EN_CCT_NONE)
        , pinyinCandidateCount(1)
        , simpleChCandidateCount(1)
        , attrib(0) 
    {}

    CharCata                    cata;
    wchar_t                     code;
    wchar_t                     tranformedCode;
    std::wstring                pinyinWithoutTone;
    unsigned int                tone;
    unsigned int                pinyinCandidateCount;
    unsigned int                simpleChCandidateCount;
    unsigned int                attrib;
};

typedef std::vector<ChChar>     ChString;

class FHTable;
class ULTable;
class TSTable;
class SurnamePinyinTable;
class IUnicodePinyinTable;
class IPhrasePinyinTable;

class UserConfig;

class ChStrParser
{
public:
    ChStrParser(const std::wstring &chinsedStr);
    ~ChStrParser();

public:
    ErrorCode GetSString(std::wstring &sStr);
    ErrorCode GetFullPinyinString(std::wstring &fullPinyinStr);
    ErrorCode GetFullPinyinStringWithTone(std::wstring &fullPinyinStr);
    ErrorCode GetFullPinyinStringArray(std::vector<std::wstring> &fullPinyinStrArray);
    ErrorCode GetFirstPinyinString(std::wstring &firstPinyinStr);

public:
    static ErrorCode Initialize();
    static ErrorCode Deinitialize();

private:
    ErrorCode GetSCode(unsigned int index, std::wstring &sCode);
    ErrorCode BuildChString();
    ErrorCode BuildChStringInternal();

private:
    static bool IsInitialized();

private:
    static FHTable *m_fhTable;
    static ULTable *m_ulTable;
    static TSTable *m_tsTable;
    static SurnamePinyinTable *m_surnamePinyinTable;
    static IUnicodePinyinTable *m_unicodePinyinTable;
    static IPhrasePinyinTable *m_phrasePinyinTable;

    UserConfig &m_userConfig;

    std::wstring m_inputStr;
    ChString m_chStr;
    bool m_isChStrBuild;
};

} // namespace CSS {

#endif // #ifndef CHSTRPARSER_H_782384230059046783587278472747889340696897956807680989689
