#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CInputDialog dialog

class CInputDialog : public CDialog
{
	DECLARE_DYNCREATE(CInputDialog)

public:
	CInputDialog(
        CWnd* pParent = NULL, 
        const CString &strTitle = CString(), 
        const CString &strPrompt = CString(), 
        const CString &strDefInputResult = CString()
    );   // standard constructor
	virtual ~CInputDialog();
// Overrides
	void OnOK();
	void OnCancel();

// Dialog Data
	enum { IDD = IDD_INPUT };

    CString GetInputResult() { return m_strInputResult; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
    CString m_strInputResult;
    CString m_strTitle;
    CString m_strPrompt;
};
