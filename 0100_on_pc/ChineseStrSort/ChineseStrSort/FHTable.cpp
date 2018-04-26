#include "FHTable.h"

namespace CSS {

static const char *CSS_FHTABLE_FILE_PATH = "data/FHTable.txt";

FHTable::FHTable()
    : m_fhMap()
    , m_dataFilePath(CSS_FHTABLE_FILE_PATH)
    , m_curLineNo(0)
{

}

FHTable::~FHTable()
{

}

ErrorCode FHTable::Translate(wchar_t fromFullCodeChar, wchar_t &toHalfCodeChar)
{
    ErrorCode ec = EC_OK;
    FHMap::iterator it;

    toHalfCodeChar = fromFullCodeChar;

    it = m_fhMap.find(fromFullCodeChar);
    if (it != m_fhMap.end()) {
        toHalfCodeChar = it->second;
    }

    return ec;
}

ErrorCode FHTable::LoadData()
{
    ErrorCode ec = EC_OK;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    if (ec == EC_OK) {
        fp = fopen(m_dataFilePath.c_str(), "rb");
        if (fp == NULL) {
            ec = EC_FAILED_TO_OPEN_FHTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            ec = EC_INVALID_FORMAT_OF_FHTABLE_FILE;
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
                    ec = EC_FAILED_TO_READ_FHTABLE_FILE;
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

ErrorCode FHTable::ParseLine(const std::wstring &line)
{
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> segments;

    if (ec == EC_OK) {
        segments = SplitString(line);
        if (segments.size() != 2) {
            ec = EC_INVALID_LINE_IN_FHTABLE_FILE;
        } else if (segments[0].length() != 1) {
            ec = EC_INVALID_LINE_IN_FHTABLE_FILE;
        } else if (segments[1].length() != 1) {
            ec = EC_INVALID_LINE_IN_FHTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        wchar_t key = segments[0][0];
        wchar_t value = segments[1][0];
        m_fhMap[key] = value;
    }

    return ec;
}

ErrorCode FHTable::UnloadData()
{
    ErrorCode ec = EC_OK;

    // do nothing

    return ec;
}

} // namespace CSS {
