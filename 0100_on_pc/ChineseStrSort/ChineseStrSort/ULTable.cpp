#include "ULTable.h"

namespace CSS {

static const char *CSS_ULTABLE_FILE_PATH = "data/ULTable.txt";

ULTable::ULTable()
    : m_ulMap()
    , m_dataFilePath(CSS_ULTABLE_FILE_PATH)
    , m_curLineNo(0)
{

}

ULTable::~ULTable()
{

}

ErrorCode ULTable::Translate(wchar_t fromUpperChar, wchar_t &toLowerChar)
{
    ErrorCode ec = EC_OK;
    ULMap::iterator it;

    toLowerChar = fromUpperChar;

    it = m_ulMap.find(fromUpperChar);
    if (it != m_ulMap.end()) {
        toLowerChar = it->second;
    }

    return ec;
}

ErrorCode ULTable::LoadData()
{
    ErrorCode ec = EC_OK;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    if (ec == EC_OK) {
        fp = fopen(m_dataFilePath.c_str(), "rb");
        if (fp == NULL) {
            ec = EC_FAILED_TO_OPEN_ULTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            ec = EC_INVALID_FORMAT_OF_ULTABLE_FILE;
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
                    ec = EC_FAILED_TO_READ_ULTABLE_FILE;
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

ErrorCode ULTable::ParseLine(const std::wstring &line)
{
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> segments;

    if (ec == EC_OK) {
        segments = SplitString(line);
        if (segments.size() != 2) {
            ec = EC_INVALID_LINE_IN_ULTABLE_FILE;
        } else if (segments[0].length() != 1) {
            ec = EC_INVALID_LINE_IN_ULTABLE_FILE;
        } else if (segments[1].length() != 1) {
            ec = EC_INVALID_LINE_IN_ULTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        wchar_t key = segments[0][0];
        wchar_t value = segments[1][0];
        m_ulMap[key] = value;
    }

    return ec;
}

ErrorCode ULTable::UnloadData()
{
    ErrorCode ec = EC_OK;

    // do nothing

    return ec;
}

} // namespace CSS {
