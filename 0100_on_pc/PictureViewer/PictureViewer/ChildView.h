
// ChildView.h : interface of the CChildView class
//


#pragma once


// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnFileOpen();
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnFileReloadConfig();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnOperateBgcolor();
    afx_msg void OnOperateGlobalAlpha();
    afx_msg void OnOperateNext();
    afx_msg void OnOperatePrevious();
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnOperateRefresh();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
    virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    
private:
    int GetScrollDelta(UINT nSBCode, UINT nPos, int nBar);
public:
    afx_msg void OnFileExport();

private:
    CString m_sLastSavedFileName;
public:
    afx_msg void OnFileCloseFile();
};

