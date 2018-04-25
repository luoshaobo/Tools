
// PictureViewer.h : main header file for the PictureViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPictureViewerApp:
// See PictureViewer.cpp for the implementation of this class
//

class CPictureViewerApp : public CWinAppEx
{
public:
	CPictureViewerApp();
    ~CPictureViewerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
    ULONG_PTR m_gdiplusToken;
};

extern CPictureViewerApp theApp;
