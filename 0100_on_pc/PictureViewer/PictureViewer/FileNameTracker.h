#pragma once
#include <vector>

class CFileNameTracker
{
public:
    CFileNameTracker(void);
    ~CFileNameTracker(void);

    BOOL SetPathName(const CString &strPathName);
    void Reset();
    void ResetIndex();
    BOOL GetDirName(CString &strDirName);
    BOOL GetFileName(CString &strFileName);

    BOOL IsSameDir(const CString &strAnotherPathName);
    BOOL IsSamePathName(const CString &strAnotherPathName);
    BOOL GetCurrentFileName(CString &strCurrentFileName);
    BOOL GetCurrentPathName(CString &strCurrentPathName);

    BOOL GoPrevious(CString &strPathName);
    BOOL GoNext(CString &strPathName);

private:
    static BOOL GetDirName(const CString &strPathName, CString &strDirName);
    static BOOL GetFileName(const CString &strPathName, CString &strFileName);
    static CString MakePathName(const CString &strDirName, const CString &strFileName);

    void Init();

private:
    CString m_strPathName;
    CString m_strDirName;
    CString m_strFileName;

    std::vector<CString> m_vsAllFileNames;
    unsigned long m_nCurrentFileNameIndex;
};
