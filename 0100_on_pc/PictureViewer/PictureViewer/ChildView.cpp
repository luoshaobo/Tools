
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "PictureViewer.h"
#include "ChildView.h"
#include "Worker.h"
#include "InputDialog.h"
#include "Tools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_DRAWINGINFO                          (WM_USER + 1)


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    ON_COMMAND(ID_FILE_OPEN, &CChildView::OnFileOpen)
    ON_WM_DROPFILES()
    ON_COMMAND(ID_FILE_RELOADCONFIG, &CChildView::OnFileReloadConfig)
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_OPERATE_BGCOLOR, &CChildView::OnOperateBgcolor)
    ON_COMMAND(ID_OPERATE_GLOBALALPHA, &CChildView::OnOperateGlobalAlpha)
    ON_COMMAND(ID_OPERATE_NEXT, &CChildView::OnOperateNext)
    ON_COMMAND(ID_OPERATE_PREVIOUS, &CChildView::OnOperatePrevious)
    ON_WM_MOUSEWHEEL()
    ON_WM_KEYDOWN()
    ON_COMMAND(ID_OPERATE_REFRESH, &CChildView::OnOperateRefresh)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_CREATE()
    ON_COMMAND(ID_FILE_EXPORT, &CChildView::OnFileExport)
    ON_COMMAND(ID_FILE_CLOSE_FILE, &CChildView::OnFileCloseFile)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE | WS_EX_ACCEPTFILES;
	cs.style &= ~WS_BORDER;
    cs.style |= WS_HSCROLL | WS_VSCROLL;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), 0/*reinterpret_cast<HBRUSH>(COLOR_WINDOW+1)*/, NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
    {
        CWorker::GetInstance().OnPaint(dc);
    }
	
	// Do not call CWnd::OnPaint() for painting messages
}


void CChildView::OnFileOpen()
{
    // TODO: Add your command handler code here
    CString strTitle;

    CFileDialog fd(
        TRUE,
        NULL,
        NULL,
        0,
        NULL,
        this,
        0,
        TRUE
    );

    INT_PTR  bRet = fd.DoModal();
    if (bRet != IDOK) {
        return ;
    }

    CString strImageFilePath = fd.GetPathName();
    if (!CWorker::GetInstance().CheckImageFilePathValid(strImageFilePath)) {
        MessageBox(_T("It is not a valid image file!"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    CWorker::GetInstance().SetImageFilePath(strImageFilePath);
    Invalidate();
    
    strTitle.Format(_T("PictureViewer - %s"), (LPCTSTR)strImageFilePath);
    GetParent()->SetWindowText(strTitle);
}

void CChildView::OnFileExport()
{
    // TODO: Add your command handler code here
    CFileDialog fd(
        FALSE,
        _T("bmp"),
        m_sLastSavedFileName,
        0,
        _T(   "Bitmap Files (32 Bits) (*.bmp)|*.bmp")
          _T("|Raw Files (32 Bits) (*.data)|*.data")
          _T("|JPEG Files (*.jpg)|*.jpg")
          _T("|GIF Files (*.gif)|*.gif")
          _T("|TIFF Files (*.tiff)|*.tiff")
          _T("|PNG Files (*.png)|*.png")
          _T("||"),
        this,
        0,
        TRUE
    );

    INT_PTR  bRet = fd.DoModal();
    if (bRet != IDOK) {
        return;
    }

    CString strImageFilePath = fd.GetPathName();
    std::string sExtFilename = TK_Tools::GetExtFileName(TK_Tools::wstr2str((const wchar_t *)strImageFilePath).c_str());
    m_sLastSavedFileName = strImageFilePath;

    if (TK_Tools::LowerCase(sExtFilename) == "bmp") {
        if (!CWorker::GetInstance().SaveAsBmpFileWithColor(strImageFilePath)) {
            MessageBox(_T("Failed to save as a BMP file!"), _T("Error"), MB_OK | MB_ICONERROR);
            return;
        }
    } else if (TK_Tools::LowerCase(sExtFilename) == "data") {
        if (!CWorker::GetInstance().SaveAsRawFile(strImageFilePath)) {
            MessageBox(_T("Failed to save as a RAW file!"), _T("Error"), MB_OK | MB_ICONERROR);
            return;
        }
    
    } else if (TK_Tools::LowerCase(sExtFilename) == "jpg") {
        if (!CWorker::GetInstance().SaveAsImageFile(_T("image/jpeg"), strImageFilePath)) {
            MessageBox(_T("Failed to save as a JPEG file!"), _T("Error"), MB_OK | MB_ICONERROR);
            return;
        }
    } else if (TK_Tools::LowerCase(sExtFilename) == "gif") {
        if (!CWorker::GetInstance().SaveAsImageFile(_T("image/gif"), strImageFilePath)) {
            MessageBox(_T("Failed to save as a GIF file!"), _T("Error"), MB_OK | MB_ICONERROR);
            return;
        }
    } else if (TK_Tools::LowerCase(sExtFilename) == "tiff") {
        if (!CWorker::GetInstance().SaveAsImageFile(_T("image/tiff"), strImageFilePath)) {
            MessageBox(_T("Failed to save as a TIFF file!"), _T("Error"), MB_OK | MB_ICONERROR);
            return;
        }
    } else if (TK_Tools::LowerCase(sExtFilename) == "png") {
        if (!CWorker::GetInstance().SaveAsImageFile(_T("image/png"), strImageFilePath)) {
            MessageBox(_T("Failed to save as a PNG file!"), _T("Error"), MB_OK | MB_ICONERROR);
            return;
        }

    } else {
        MessageBox(_T("The type is not supported!"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    Invalidate();
}

void CChildView::OnDropFiles(HDROP hDropInfo)
{
    // TODO: Add your message handler code here and/or call default
    CString strTitle;
    TCHAR szFilePathName[_MAX_PATH+1] = { 0 };     
   
    UINT nNumOfFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
    if (nNumOfFiles >= 1) {
        DragQueryFile(hDropInfo, 0, (LPTSTR)szFilePathName, _MAX_PATH);
        if (!CWorker::GetInstance().CheckImageFilePathValid(szFilePathName)) {
            MessageBox(_T("It is not a valid image file!"), _T("Error"), MB_OK | MB_ICONERROR);
        } else {
            CWorker::GetInstance().SetImageFilePath(szFilePathName);
            Invalidate();
            strTitle.Format(_T("PictureViewer - %s"), szFilePathName);
            GetParent()->SetWindowText(strTitle);
        }
    }
    DragFinish(hDropInfo);

    CWnd::OnDropFiles(hDropInfo);
}

void CChildView::OnFileReloadConfig()
{
    // TODO: Add your command handler code here
    BOOL bSuc;

    bSuc = CWorker::GetInstance().ReloadConfig();
    if (!bSuc) {
        MessageBox(_T("Reloading config is failed!"), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }
    Invalidate();
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    CWorker::GetInstance().SetWatchPoint(point.x, point.y);

    CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnOperateBgcolor()
{
    // TODO: Add your command handler code here
    CColorDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        COLORREF color = dlg.GetColor();
        CWorker::GetInstance().SetPictureBgColor(255, GetRValue(color), GetGValue(color), GetBValue(color));
        Invalidate();
    }
}

void CChildView::OnOperateGlobalAlpha()
{
    // TODO: Add your command handler code here
    CString strMessage;
    CString strDefInputResult;

    strDefInputResult.Format(_T("%u"), CWorker::GetInstance().GetGlobalAlpha());
    CInputDialog dlg(this, _T("Set Global Alpha"), _T("Please enter an integer within [0,255]:"), strDefInputResult);
    if (dlg.DoModal() == IDOK)
    {
        CString strResult = dlg.GetInputResult();
        BYTE nGlobalAlpha = (BYTE)TK_Tools::StrToUL(TK_Tools::wstr2str(LPCTSTR(strResult)));
        strMessage.Format(_T("The global alpha will be changed to 0x%02X."), nGlobalAlpha);
        MessageBox(strMessage, _T("Info"), MB_OK | MB_ICONINFORMATION);
        CWorker::GetInstance().SetGlobalAlpha(nGlobalAlpha);
        Invalidate();
    }
}

void CChildView::OnOperateNext()
{
    // TODO: Add your command handler code here
    CString strTitle;
    BOOL bSuc;
    bSuc = CWorker::GetInstance().ShowNextPicture();
    if (!bSuc) {
        MessageBox(_T("Alread the last picture!"), _T("Info"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    Invalidate();
    strTitle.Format(_T("PictureViewer - %s"), CWorker::GetInstance().GetImageFilePath());
    GetParent()->SetWindowText(strTitle);
}

void CChildView::OnOperatePrevious()
{
    // TODO: Add your command handler code here
    CString strTitle;
    BOOL bSuc;
    bSuc = CWorker::GetInstance().ShowPreviousPicture();
    if (!bSuc) {
        MessageBox(_T("Alread the first picture!"), _T("Info"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    Invalidate();
    strTitle.Format(_T("PictureViewer - %s"), CWorker::GetInstance().GetImageFilePath());
    GetParent()->SetWindowText(strTitle);
}

void CChildView::OnOperateRefresh()
{
    // TODO: Add your command handler code here
    CString strTitle;
    BOOL bSuc;
    bSuc = CWorker::GetInstance().RefreshPictureDir();
    if (!bSuc) {
        MessageBox(_T("No picture is found!"), _T("Info"), MB_OK | MB_ICONINFORMATION);
        return;
    }
    Invalidate();
    strTitle.Format(_T("PictureViewer - %s"), CWorker::GetInstance().GetImageFilePath());
    GetParent()->SetWindowText(strTitle);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: Add your message handler code here and/or call default
    float fScaleFactor = CWorker::GetInstance().GetScaleFactor();
    if ((nFlags & MK_SHIFT) && (nFlags & MK_CONTROL)) {
        fScaleFactor += zDelta / 120.0f / 10000.0f;
    } else if (nFlags & MK_SHIFT) {
        fScaleFactor += zDelta / 120.0f / 1000.0f;
    } else if (nFlags & MK_CONTROL) {
        fScaleFactor += zDelta / 120.0f / 100.0f;
    } else {
        fScaleFactor += zDelta / 120.0f / 10.0f;
    }
    if (fScaleFactor <= 0.0001f) {
        fScaleFactor = 0.0001f;
    }
    if (fScaleFactor >= 10000.0f) {
        fScaleFactor = 10000.0f;
    }

    CWorker::GetInstance().SetScaleFactor(fScaleFactor);
    Invalidate();

    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: Add your message handler code here and/or call default
    switch (nChar) {
    case ' ':
        {
            OnOperateNext();
            goto PROCESSED;
        }
        break;
    case VK_BACK:
        {
            OnOperatePrevious();
            goto PROCESSED;
        }
        break;
    case 'F':
    case 'f':
        {
            OnOperateRefresh();
            goto PROCESSED;
        }
        break;
    default:
        break;
    }

    CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

PROCESSED:
    return;
}

int CChildView::GetScrollDelta(UINT nSBCode, UINT nPos, int nBar)
{
    int nResult = 0;

    switch (nSBCode) {
    case SB_LINELEFT:
    //case SB_LINEUP:
        nResult = 1;
        break;
    case SB_LINERIGHT:
    //case SB_LINEDOWN:
        nResult = -1;
        break;
    case SB_PAGELEFT:
    //case SB_PAGEUP:
        nResult = 10;
        break;
    case SB_PAGERIGHT:
    //case SB_PAGEDOWN:
        nResult = -10;
        break;
    case SB_LEFT:
    //case SB_TOP:
        nResult = 0;
        break;
    case SB_RIGHT:
    //case SB_BOTTOM:
        nResult = 0; // ?
        break;
    default:
        break;
    }

    return nResult;
}

void CChildView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK) {
        CWorker::GetInstance().SetOriginX(0 - nPos);
    } else {
        CWorker::GetInstance().MoveOrigin(GetScrollDelta(nSBCode, nPos, SB_HORZ), 0);
    }
    Invalidate();

    CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CChildView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    if (nSBCode == SB_THUMBPOSITION || nSBCode == SB_THUMBTRACK) {
        CWorker::GetInstance().SetOriginY(0 - nPos);
    } else {
        CWorker::GetInstance().MoveOrigin(0, GetScrollDelta(nSBCode, nPos, SB_VERT));
    }
    Invalidate();

    CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CChildView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: Add your specialized code here and/or call the base class
    if (message == WM_DRAWINGINFO) {
        CWorker::DrawingInfoData *pdid = (CWorker::DrawingInfoData *)lParam;
        SetScrollInfo(SB_HORZ, &pdid->siHorizon);
        SetScrollInfo(SB_VERT, &pdid->siVertical);

        DBG_TRACE("###INFO: [%s()] SB_HORZ: fMask=0x%04X, nMin=%d, nMax=%d, nPage=%d, nPos=%d\n", 
            __FUNCTION__,
            pdid->siHorizon.fMask,
            pdid->siHorizon.nMin,
            pdid->siHorizon.nMax,
            pdid->siHorizon.nPage,
            pdid->siHorizon.nPos
        );

        DBG_TRACE("###INFO: [%s()] SB_VERT: fMask=0x%04X, nMin=%d, nMax=%d, nPage=%d, nPos=%d\n", 
            __FUNCTION__,
            pdid->siVertical.fMask,
            pdid->siVertical.nMin,
            pdid->siVertical.nMax,
            pdid->siVertical.nPage,
            pdid->siVertical.nPos
        );

        *pResult = TRUE;
        return TRUE;
    }
    
    return CWnd::OnWndMsg(message, wParam, lParam, pResult);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CWorker::GetInstance().RegisterDrawingInfoMonitor(GetSafeHwnd(), WM_DRAWINGINFO);

    return 0;
}

void CChildView::OnFileCloseFile()
{
    // TODO: Add your command handler code here
    CWorker::GetInstance().CloseCurrentFile();
    Invalidate();
}
