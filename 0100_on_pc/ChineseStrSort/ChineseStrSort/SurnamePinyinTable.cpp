#include "SurnamePinyinTable.h"

namespace CSS {

static const char *CSS_SURNAMEPINYINTABLE_FILE_PATH = "data/SurnamePinyinTable.txt";

SurnamePinyinTable::SurnamePinyinTable()
    : m_surnamePinyinMap()
    , m_dataFilePath(CSS_SURNAMEPINYINTABLE_FILE_PATH)
    , m_curLineNo(0)
{

}

SurnamePinyinTable::~SurnamePinyinTable()
{

}

ErrorCode SurnamePinyinTable::Translate(const std::wstring &surname, std::vector<std::wstring> &pingyins)
{
    ErrorCode ec = EC_OK;
    SurnamePinyinMap::iterator it;

    pingyins.clear();

    it = m_surnamePinyinMap.find(surname.substr(0, 2));
    if (it != m_surnamePinyinMap.end()) {
        pingyins = it->second;
    } else {
        it = m_surnamePinyinMap.find(surname.substr(0, 1));
        if (it != m_surnamePinyinMap.end()) {
            pingyins = it->second;
        } 
    }

    return ec;
}

ErrorCode SurnamePinyinTable::LoadData()
{
    ErrorCode ec = EC_OK;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    if (ec == EC_OK) {
        fp = fopen(m_dataFilePath.c_str(), "rb");
        if (fp == NULL) {
            ec = EC_FAILED_TO_OPEN_SURNAMEPINYINTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            ec = EC_INVALID_FORMAT_OF_SURNAMEPINYINTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        while (true) {
            m_curLineNo++;
            returnedLine = fgetws(lineBuf, BUF_LEN_MAX, fp);
            if (returnedLine == NULL) {
                if (feof(fp)) {
                    // do nothing
                } else {
                    ec = EC_FAILED_TO_READ_SURNAMEPINYINTABLE_FILE;
                }
                break;
            } else {
                trimedLine = TrimAll(returnedLine);
                if (trimedLine.length() == 0) {
                    continue;
                }
                ec = ParseLine(returnedLine);
                if (ec != EC_OK) {
                    break;
                }
            }
        }
    }

    if (fp != NULL) {
        fclose(fp);
        fp = NULL;
    }

    if (ec != EC_OK) {
        CSS_ERROR("%s(): failed to parse \"%s\"! [LastLineNo=%u]", CSS_CURFUNC, m_dataFilePath.c_str(), m_curLineNo);
    }
    return ec;
}

ErrorCode SurnamePinyinTable::ParseLine(const std::wstring &line)
{
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> segments;
    unsigned int i;

    if (ec == EC_OK) {
        segments = SplitString(line);
        if (segments.size() < 2) {
            ec = EC_INVALID_LINE_IN_SURNAMEPINYINTABLE_FILE;
        } else {
            for (i = 1; i < segments.size(); ++i) {
                if (!IsPinyinStr(segments[i])) {
                    ec = EC_INVALID_LINE_IN_UNICODEPINYINTABLE_FILE;
                }
            }
        }
    }

    if (ec == EC_OK) {
        std::wstring key = segments[0];
        std::vector<std::wstring> value;
        for (i = 1; i < segments.size(); ++i) {
            value.push_back(segments[i]);
        }
        m_surnamePinyinMap[key] = value;
    }

    return ec;
}

ErrorCode SurnamePinyinTable::UnloadData()
{
    ErrorCode ec = EC_OK;

    // do nothing

    return ec;
}

} // namespace CSS {
