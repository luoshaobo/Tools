#include "StdAfx.h"
#include <algorithm>
#include "FileNameTracker.h"

CFileNameTracker::CFileNameTracker(void)
{
    Init();
}

CFileNameTracker::~CFileNameTracker(void)
{
}

void CFileNameTracker::Init()
{
    m_strPathName = _T("***InvalidPathName***");
    m_strDirName = _T("***InvalidDirName***");
    m_strFileName = _T("***InvalidFileName***");

    m_vsAllFileNames.clear();
    m_nCurrentFileNameIndex = 0;
}

BOOL CFileNameTracker::GetDirName(const CString &strPathName, CString &strDirName)
{
    strDirName = "";

    int nPos;

    nPos = strPathName.ReverseFind(_T('\\'));
    if (nPos == -1) {
        strDirName = "";
        return TRUE;
    }

    strDirName = strPathName.Mid(0, nPos);
    return TRUE;
}

BOOL CFileNameTracker::GetFileName(const CString &strPathName, CString &strFileName)
{
    strFileName = "";

    int nPos;

    nPos = strPathName.ReverseFind(_T('\\'));
    if (nPos == -1) {
        strFileName = strPathName;
        return TRUE;
    }

    strFileName = strPathName.Mid(nPos + 1);
    return TRUE;
}

CString CFileNameTracker::MakePathName(const CString &strDirName, const CString &strFileName)
{
    CString strPathName;

    strPathName.Format(_T("%s\\%s"), strDirName, strFileName);
    return strPathName;
}

static bool CStringNoCaseComp(const CString &strArg1, const CString &strArg2)
{
    return strArg1.CompareNoCase(strArg2) < 0;
}

BOOL CFileNameTracker::SetPathName(const CString &strPathName)
{
    BOOL bRet = FALSE;

    Init();

    m_strPathName = strPathName;

    bRet = GetDirName(m_strPathName, m_strDirName);
    if (!bRet) {
        goto FAILED;
    }

    bRet = GetFileName(m_strPathName, m_strFileName);
    if (!bRet) {
        goto FAILED;
    }
    
    {
        CString strSearchPattern;
        strSearchPattern.Format(_T("%s\\*.*"), m_strDirName);
        CFileFind finder;
        BOOL bWorking = finder.FindFile(strSearchPattern);
        while (bWorking) {
            bWorking = finder.FindNextFile();
            CString strFileName = finder.GetFileName();
            if (!finder.IsDirectory()) {
                m_vsAllFileNames.push_back(strFileName);
            }
        }
        finder.Close();

        std::sort(m_vsAllFileNames.begin(), m_vsAllFileNames.end(), &CStringNoCaseComp);
        std::vector<CString>::iterator it = std::find(m_vsAllFileNames.begin(), m_vsAllFileNames.end(), m_strFileName);
        if (it == m_vsAllFileNames.end()) {
            m_nCurrentFileNameIndex = 0;
        } else {
            m_nCurrentFileNameIndex = it - m_vsAllFileNames.begin();
        }
    }

    bRet = TRUE;

FAILED:
    return bRet;
}

void CFileNameTracker::Reset()
{
    Init();
}

void CFileNameTracker::ResetIndex()
{
    m_nCurrentFileNameIndex = 0;
}

BOOL CFileNameTracker::GetDirName(CString &strDirName)
{
    strDirName = m_strDirName;

    return TRUE;
}

BOOL CFileNameTracker::GetFileName(CString &strFileName)
{
    strFileName = m_strFileName;

    return TRUE;
}

BOOL CFileNameTracker::IsSameDir(const CString &strAnotherPathName)
{
    BOOL bRet = FALSE;
    CString strAnotherDirName;

    bRet = GetDirName(strAnotherPathName, strAnotherDirName);
    if (!bRet) {
        goto FAILED;
    }

    bRet = (strAnotherDirName == m_strDirName);

FAILED:
    return bRet;
}

BOOL CFileNameTracker::IsSamePathName(const CString &strAnotherPathName)
{
    return (strAnotherPathName == m_strPathName);
}

BOOL CFileNameTracker::GetCurrentFileName(CString &strCurrentFileName)
{
    strCurrentFileName = "";

    if (m_nCurrentFileNameIndex >= m_vsAllFileNames.size()) {
        return FALSE;
    }

    strCurrentFileName = m_vsAllFileNames[m_nCurrentFileNameIndex];
    return TRUE;
}

BOOL CFileNameTracker::GetCurrentPathName(CString &strCurrentPathName)
{
    strCurrentPathName = "";

    if (m_nCurrentFileNameIndex >= m_vsAllFileNames.size()) {
        return FALSE;
    }
    
    strCurrentPathName = MakePathName(m_strDirName, m_vsAllFileNames[m_nCurrentFileNameIndex]);
    return TRUE;
}

BOOL CFileNameTracker::GoPrevious(CString &strPathName)
{
    strPathName = "";

    if (m_nCurrentFileNameIndex == 0 || m_nCurrentFileNameIndex >= m_vsAllFileNames.size()) {
        return FALSE;
    }
    
    m_nCurrentFileNameIndex--;
    strPathName = MakePathName(m_strDirName, m_vsAllFileNames[m_nCurrentFileNameIndex]);
    return TRUE;
}

BOOL CFileNameTracker::GoNext(CString &strPathName)
{
    strPathName = "";

    if (m_nCurrentFileNameIndex + 1 >= m_vsAllFileNames.size()) {
        return FALSE;
    }
    
    m_nCurrentFileNameIndex++;
    strPathName = MakePathName(m_strDirName, m_vsAllFileNames[m_nCurrentFileNameIndex]);
    return TRUE;
}

