// InputDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PictureViewer.h"
#include "InputDialog.h"


// CInputDialog dialog

IMPLEMENT_DYNCREATE(CInputDialog, CDialog)

CInputDialog::CInputDialog(CWnd* pParent, const CString &strTitle, const CString &strPrompt, const CString &strDefInputResult)
	: CDialog(CInputDialog::IDD, pParent), m_strTitle(strTitle), m_strPrompt(strPrompt), m_strInputResult(strDefInputResult)
{

}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetWindowText(m_strTitle);
    SetDlgItemText(IDC_INPUT_PROMPT, m_strPrompt);
    SetDlgItemText(IDC_INPUT_RESULT, m_strInputResult);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
END_MESSAGE_MAP()

// CInputDialog message handlers

void CInputDialog::OnOK()
{
    GetDlgItemText(IDC_INPUT_RESULT, m_strInputResult);
    CDialog::OnOK();
}

void CInputDialog::OnCancel()
{
    CDialog::OnCancel();
}
