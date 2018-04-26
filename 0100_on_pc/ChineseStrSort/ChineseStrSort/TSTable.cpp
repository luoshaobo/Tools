#include "TSTable.h"

namespace CSS {

static const char *CSS_TSTABLE_FILE_PATH = "data/TSTable.txt";

TSTable::TSTable()
    : m_tsMap()
    , m_dataFilePath(CSS_TSTABLE_FILE_PATH)
    , m_curLineNo(0)
{

}

TSTable::~TSTable()
{

}

ErrorCode TSTable::Translate(wchar_t fromTradChar, wchar_t &toSimpleChar, unsigned int &candidateCount)
{
    ErrorCode ec = EC_OK;
    TSMap::iterator it;

    toSimpleChar = fromTradChar;
    candidateCount = 1;

    it = m_tsMap.find(fromTradChar);
    if (it != m_tsMap.end()) {
        toSimpleChar = it->second[0];
        candidateCount = it->second.size();
    }

    return ec;
}

ErrorCode TSTable::LoadData()
{
    ErrorCode ec = EC_OK;
    FILE *fp = NULL;
    wchar_t lineBuf[BUF_LEN_MAX];
    wchar_t *returnedLine;
    std::wstring trimedLine;

    if (ec == EC_OK) {
        fp = fopen(m_dataFilePath.c_str(), "rb");
        if (fp == NULL) {
            ec = EC_FAILED_TO_OPEN_TSTABLE_FILE;
        }
    }

    if (ec == EC_OK) {
        size_t readCount = fread(lineBuf, 1, 2, fp);
        if (readCount != 2 || lineBuf[0] != 0xFEFF) {
            ec = EC_INVALID_FORMAT_OF_TSTABLE_FILE;
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
                    ec = EC_FAILED_TO_READ_TSTABLE_FILE;
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

ErrorCode TSTable::ParseLine(const std::wstring &line)
{
    ErrorCode ec = EC_OK;
    std::vector<std::wstring> segments;
    unsigned int i;

    if (ec == EC_OK) {
        segments = SplitString(line);
        if (segments.size() < 2) {
            ec = EC_INVALID_LINE_IN_TSTABLE_FILE;
        } else if (segments[0].length() != 1) {
            ec = EC_INVALID_LINE_IN_TSTABLE_FILE;
        } else if (segments[1].length() != 1) {
            ec = EC_INVALID_LINE_IN_TSTABLE_FILE;
        } else {
            for (i = 2; i < segments.size(); ++i) {
                if (segments[i].length() != 1) {
                    ec = EC_INVALID_LINE_IN_TSTABLE_FILE;
                }
            }
        }
    }

    if (ec == EC_OK) {
        wchar_t key = segments[0][0];
        std::vector<wchar_t> value;
        for (i = 1; i < segments.size(); ++i) {
            value.push_back(segments[i][0]);
        }
        if (value.size() > 0) {
            m_tsMap[key] = value;
        }
    }

    return ec;
}

ErrorCode TSTable::UnloadData()
{
    ErrorCode ec = EC_OK;

    // do nothing

    return ec;
}

} // namespace CSS {
