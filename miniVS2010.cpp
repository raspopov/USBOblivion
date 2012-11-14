// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "comdef.h"
#include "afxglobals.h"
#include "afxctrlcontainer.h"

extern CObList afxAllToolBars;

BOOL CMemDC::m_bUseMemoryDC = TRUE;

static const CString strOfficeFontName = _T("Tahoma");
static const CString strOffice2007FontName = _T("Segoe UI");
static const CString strDefaultFontName = _T("MS Sans Serif");
static const CString strVertFontName = _T("Arial");
static const CString strMarlettFontName = _T("Marlett");

HINSTANCE AFX_GLOBAL_DATA::m_hinstD2DDLL = NULL;
HINSTANCE AFX_GLOBAL_DATA::m_hinstDWriteDLL = NULL;

ID2D1Factory* AFX_GLOBAL_DATA::m_pDirect2dFactory = NULL;
IDWriteFactory* AFX_GLOBAL_DATA::m_pWriteFactory = NULL;
IWICImagingFactory* AFX_GLOBAL_DATA::m_pWicFactory = NULL;

D2D1MAKEROTATEMATRIX AFX_GLOBAL_DATA::m_pfD2D1MakeRotateMatrix = NULL;

BOOL AFX_GLOBAL_DATA::m_bD2DInitialized = FALSE;

CMemDC::CMemDC(CDC& dc, CWnd* pWnd) :
	m_dc(dc), m_bMemDC(FALSE), m_hBufferedPaint(NULL), m_pOldBmp(NULL)
{
	ASSERT_VALID(pWnd);

	pWnd->GetClientRect(m_rect);

	m_rect.right += pWnd->GetScrollPos(SB_HORZ);
	m_rect.bottom += pWnd->GetScrollPos(SB_VERT);

	if (afxGlobalData.m_pfBeginBufferedPaint != NULL && afxGlobalData.m_pfEndBufferedPaint != NULL)
	{
		HDC hdcPaint = NULL;

		if (!afxGlobalData.m_bBufferedPaintInited && afxGlobalData.m_pfBufferedPaintInit != NULL && afxGlobalData.m_pfBufferedPaintUnInit != NULL)
		{
			afxGlobalData.m_pfBufferedPaintInit();
			afxGlobalData.m_bBufferedPaintInited = TRUE;
		}

		m_hBufferedPaint = (*afxGlobalData.m_pfBeginBufferedPaint)(dc.GetSafeHdc(), m_rect, AFX_BPBF_TOPDOWNDIB, NULL, &hdcPaint);

		if (m_hBufferedPaint != NULL && hdcPaint != NULL)
		{
			m_bMemDC = TRUE;
			m_dcMem.Attach(hdcPaint);
		}
	}
	else
	{
		if (m_bUseMemoryDC && m_dcMem.CreateCompatibleDC(&m_dc) && m_bmp.CreateCompatibleBitmap(&m_dc, m_rect.Width(), m_rect.Height()))
		{
			//-------------------------------------------------------------
			// Off-screen DC successfully created. Better paint to it then!
			//-------------------------------------------------------------
			m_bMemDC = TRUE;
			m_pOldBmp = m_dcMem.SelectObject(&m_bmp);
		}
	}
}

CMemDC::CMemDC(CDC& dc, const CRect& rect) :
	m_dc(dc), m_bMemDC(FALSE), m_hBufferedPaint(NULL), m_pOldBmp(NULL), m_rect(rect)
{
	ASSERT(!m_rect.IsRectEmpty());

	if (afxGlobalData.m_pfBeginBufferedPaint != NULL && afxGlobalData.m_pfEndBufferedPaint != NULL)
	{
		HDC hdcPaint = NULL;

		if (!afxGlobalData.m_bBufferedPaintInited && afxGlobalData.m_pfBufferedPaintInit != NULL && afxGlobalData.m_pfBufferedPaintUnInit != NULL)
		{
			afxGlobalData.m_pfBufferedPaintInit();
			afxGlobalData.m_bBufferedPaintInited = TRUE;
		}

		m_hBufferedPaint = (*afxGlobalData.m_pfBeginBufferedPaint)(dc.GetSafeHdc(), m_rect, AFX_BPBF_TOPDOWNDIB, NULL, &hdcPaint);

		if (m_hBufferedPaint != NULL && hdcPaint != NULL)
		{
			m_bMemDC = TRUE;
			m_dcMem.Attach(hdcPaint);
		}
	}
	else
	{
		if (m_bUseMemoryDC && m_dcMem.CreateCompatibleDC(&m_dc) && m_bmp.CreateCompatibleBitmap(&m_dc, m_rect.Width(), m_rect.Height()))
		{
			//-------------------------------------------------------------
			// Off-screen DC successfully created. Better paint to it then!
			//-------------------------------------------------------------
			m_bMemDC = TRUE;
			m_pOldBmp = m_dcMem.SelectObject(&m_bmp);
		}
	}
}

CMemDC::~CMemDC()
{
	if (m_hBufferedPaint != NULL)
	{
		m_dcMem.Detach();
		(*afxGlobalData.m_pfEndBufferedPaint)(m_hBufferedPaint, TRUE);
	}
	else if (m_bMemDC)
	{
		//--------------------------------------
		// Copy the results to the on-screen DC:
		//--------------------------------------
		CRect rectClip;
		int nClipType = m_dc.GetClipBox(rectClip);

		if (nClipType != NULLREGION)
		{
			if (nClipType != SIMPLEREGION)
			{
				rectClip = m_rect;
			}

			m_dc.BitBlt(rectClip.left, rectClip.top, rectClip.Width(), rectClip.Height(), &m_dcMem, rectClip.left, rectClip.top, SRCCOPY);
		}

		m_dcMem.SelectObject(m_pOldBmp);
	}
}

static int CALLBACK FontFamalyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR* /*lptm*/, ULONG /*ulFontType*/, LPARAM lParam)
{
	ENSURE(lplf != NULL);
	ENSURE(lParam != NULL);

	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase((LPCTSTR) lParam) == 0 ? 0 : 1;
}

/////////////////////////////////////////////////////////////////////////////
// DLL Load Helper

inline HMODULE AfxLoadSystemLibraryUsingFullPath(_In_z_ const WCHAR *pszLibrary)
{
	WCHAR wszLoadPath[MAX_PATH+1];
	if (::GetSystemDirectoryW(wszLoadPath, _countof(wszLoadPath)) == 0)
	{
		return NULL;
	}

	if (wszLoadPath[wcslen(wszLoadPath)-1] != L'\\')
	{
		if (wcscat_s(wszLoadPath, _countof(wszLoadPath), L"\\") != 0)
		{
			return NULL;
		}
	}

	if (wcscat_s(wszLoadPath, _countof(wszLoadPath), pszLibrary) != 0)
	{
		return NULL;
	}

	return(::AfxCtxLoadLibraryW(wszLoadPath));
}

/////////////////////////////////////////////////////////////////////////////
// Cached system metrics, etc
AFX_GLOBAL_DATA afxGlobalData;

#ifdef _AFXDLL
// Reference count on global data
DWORD g_dwAfxGlobalDataRef = 0;
#endif

// Initialization code
AFX_GLOBAL_DATA::AFX_GLOBAL_DATA()
{
	// Detect the kind of OS:
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	::GetVersionEx(&osvi);

	bIsRemoteSession = GetSystemMetrics(SM_REMOTESESSION);

	bIsWindowsVista = (osvi.dwMajorVersion >= 6);
	bIsWindows7 = (osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion >= 1) || (osvi.dwMajorVersion > 6) ;
	bDisableAero = FALSE;

	m_bIsRibbonImageScale = TRUE;

	// Cached system values(updated in CWnd::OnSysColorChange)
	hbrBtnShadow = NULL;
	hbrBtnHilite = NULL;
	hbrWindow = NULL;

	UpdateSysColors();

	m_hinstUXThemeDLL = ::AfxCtxLoadLibraryW(L"UxTheme.dll");
	if (m_hinstUXThemeDLL != NULL)
	{
		m_pfDrawThemeBackground = (DRAWTHEMEPARENTBACKGROUND)::GetProcAddress(m_hinstUXThemeDLL, "DrawThemeParentBackground");
		m_pfDrawThemeTextEx = (DRAWTHEMETEXTEX)::GetProcAddress(m_hinstUXThemeDLL, "DrawThemeTextEx");

		m_pfBufferedPaintInit = (BUFFEREDPAINTINIT)::GetProcAddress(m_hinstUXThemeDLL, "BufferedPaintInit");
		m_pfBufferedPaintUnInit = (BUFFEREDPAINTUNINIT)::GetProcAddress(m_hinstUXThemeDLL, "BufferedPaintUnInit");

		m_pfBeginBufferedPaint = (BEGINBUFFEREDPAINT)::GetProcAddress(m_hinstUXThemeDLL, "BeginBufferedPaint");
		m_pfEndBufferedPaint = (ENDBUFFEREDPAINT)::GetProcAddress(m_hinstUXThemeDLL, "EndBufferedPaint");
	}
	else
	{
		m_pfDrawThemeBackground = NULL;
		m_pfDrawThemeTextEx = NULL;

		m_pfBufferedPaintInit = NULL;
		m_pfBufferedPaintUnInit = NULL;

		m_pfBeginBufferedPaint = NULL;
		m_pfEndBufferedPaint = NULL;
	}

	m_hinstDwmapiDLL = AfxLoadSystemLibraryUsingFullPath(L"dwmapi.dll");
	if (m_hinstDwmapiDLL != NULL)
	{
		m_pfDwmExtendFrameIntoClientArea = (DWMEXTENDFRAMEINTOCLIENTAREA)::GetProcAddress(m_hinstDwmapiDLL, "DwmExtendFrameIntoClientArea");
		m_pfDwmDefWindowProc = (DWMDEFWINDOWPROC) ::GetProcAddress(m_hinstDwmapiDLL, "DwmDefWindowProc");
		m_pfDwmIsCompositionEnabled = (DWMISCOMPOSITIONENABLED)::GetProcAddress(m_hinstDwmapiDLL, "DwmIsCompositionEnabled");
	}
	else
	{
		m_pfDwmExtendFrameIntoClientArea = NULL;
		m_pfDwmDefWindowProc = NULL;
		m_pfDwmIsCompositionEnabled = NULL;
	}

	m_hcurStretch = NULL;
	m_hcurStretchVert = NULL;
	m_hcurHand = NULL;
	m_hcurSizeAll = NULL;
	m_hiconTool = NULL;
	m_hiconLink = NULL;
	m_hiconColors = NULL;
	m_hcurMoveTab = NULL;
	m_hcurNoMoveTab = NULL;

	m_bUseSystemFont = FALSE;
	m_bInSettingChange = FALSE;

	UpdateFonts();
	OnSettingChange();

	m_bIsRTL = FALSE;
	m_bBufferedPaintInited = FALSE;

	m_nDragFrameThicknessFloat = 4;  // pixels
	m_nDragFrameThicknessDock = 3;   // pixels

	m_nAutoHideToolBarSpacing = 14; // pixels
	m_nAutoHideToolBarMargin  = 4;  // pixels

	m_nCoveredMainWndClientAreaPercent = 50; // percents

	m_nMaxToolTipWidth = -1;
	m_bIsBlackHighContrast = FALSE;
	m_bIsWhiteHighContrast = FALSE;

	m_bUseBuiltIn32BitIcons = TRUE;

	m_bComInitialized = FALSE;

	m_pTaskbarList = NULL;
	m_pTaskbarList3 = NULL;
	m_bTaskBarInterfacesAvailable = TRUE;

	EnableAccessibilitySupport();
}

AFX_GLOBAL_DATA::~AFX_GLOBAL_DATA()
{
	CleanUp();
}

void AFX_GLOBAL_DATA::UpdateFonts()
{
	CWindowDC dc(NULL);
	m_dblRibbonImageScale = dc.GetDeviceCaps(LOGPIXELSX) / 96.0f;

	if (m_dblRibbonImageScale > 1. && m_dblRibbonImageScale < 1.1)
	{
		m_dblRibbonImageScale = 1.;
	}

	if (fontRegular.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontRegular.Detach());
	}

	if (fontTooltip.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontTooltip.Detach());
	}

	if (fontBold.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontBold.Detach());
	}

	if (fontDefaultGUIBold.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontDefaultGUIBold.Detach());
	}

	if (fontUnderline.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontUnderline.Detach());
	}

	if (fontDefaultGUIUnderline.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontDefaultGUIUnderline.Detach());
	}

	if (fontVert.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontVert.Detach());
	}

	if (fontVertCaption.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontVertCaption.Detach());
	}

	if (fontMarlett.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontMarlett.Detach());
	}

	if (fontSmall.GetSafeHandle() != NULL)
	{
		::DeleteObject(fontSmall.Detach());
	}

	// Initialize fonts:

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);
	GetNonClientMetrics (info);

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfCharSet = (BYTE) GetTextCharsetInfo(dc.GetSafeHdc(), NULL, 0);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	//------------------
	// Adjust font size:
	//------------------
	int nFontHeight = lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight;
	if (nFontHeight <= 12)
	{
		nFontHeight = 11;
	}
	else
	{
		nFontHeight--;
	}

	lf.lfHeight = (lf.lfHeight < 0) ? -nFontHeight : nFontHeight;

	// Check if we should use system font
	lstrcpy(lf.lfFaceName, info.lfMenuFont.lfFaceName);

	BOOL fUseSystemFont = m_bUseSystemFont || (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		// Check for "Segoe UI" or "Tahoma" font existance:
		if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamalyProcFonts, (LPARAM)(LPCTSTR) strOffice2007FontName) == 0)
		{
			// Found! Use MS Office 2007 font!
			lstrcpy(lf.lfFaceName, strOffice2007FontName);
			lf.lfQuality = 5 /*CLEARTYPE_QUALITY*/;
		}
		else if (::EnumFontFamilies(dc.GetSafeHdc(), NULL, FontFamalyProcFonts, (LPARAM)(LPCTSTR) strOfficeFontName) == 0)
		{
			// Found! Use MS Office font!
			lstrcpy(lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			// Not found. Use default font:
			lstrcpy(lf.lfFaceName, strDefaultFontName);
		}
	}
	
	fontRegular.CreateFontIndirect(&lf);

	// Create small font:
	LONG lfHeightSaved = lf.lfHeight;

	lf.lfHeight = (long)((1. + abs(lf.lfHeight)) * 2 / 3);
	if (lfHeightSaved < 0)
	{
		lf.lfHeight = -lf.lfHeight;
	}

	fontSmall.CreateFontIndirect(&lf);
	lf.lfHeight = lfHeightSaved;

	// Create tooltip font:
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	GetNonClientMetrics (ncm);

	lf.lfItalic = ncm.lfStatusFont.lfItalic;
	lf.lfWeight = ncm.lfStatusFont.lfWeight;
	fontTooltip.CreateFontIndirect(&lf);

	lf.lfItalic = info.lfMenuFont.lfItalic;
	lf.lfWeight = info.lfMenuFont.lfWeight;

	// Create "underline" font:
	lf.lfUnderline = TRUE;
	fontUnderline.CreateFontIndirect(&lf);
	lf.lfUnderline = FALSE;

	// Create bold font:
	lf.lfWeight = FW_BOLD;
	fontBold.CreateFontIndirect(&lf);

	// Create Marlett font:
	BYTE bCharSet = lf.lfCharSet;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfCharSet = SYMBOL_CHARSET;
	lf.lfWeight = 0;
	lf.lfHeight = ::GetSystemMetrics(SM_CYMENUCHECK) - 1;
	lstrcpy(lf.lfFaceName, strMarlettFontName);

	fontMarlett.CreateFontIndirect(&lf);
	lf.lfCharSet = bCharSet; // Restore charset

	// Create vertical font:
	CFont font;
	if (font.CreateStockObject(DEFAULT_GUI_FONT))
	{
		if (font.GetLogFont(&lf) != 0)
		{
			lf.lfOrientation = 900;
			lf.lfEscapement = 2700;

			lf.lfHeight = info.lfMenuFont.lfHeight;
			lf.lfWeight = info.lfMenuFont.lfWeight;
			lf.lfItalic = info.lfMenuFont.lfItalic;

			{
				lstrcpy(lf.lfFaceName, strVertFontName);
			}

			fontVert.CreateFontIndirect(&lf);

			lf.lfEscapement = 900;
			fontVertCaption.CreateFontIndirect(&lf);
		}
	}

	// Create dialog underline and bold fonts:
	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	ASSERT_VALID(pDefaultGUIFont);
	pDefaultGUIFont->GetLogFont(&lf);

	lf.lfUnderline = TRUE;
	fontDefaultGUIUnderline.CreateFontIndirect(&lf);
	lf.lfUnderline = FALSE;

	lf.lfWeight = FW_BOLD;
	fontDefaultGUIBold.CreateFontIndirect(&lf);

	UpdateTextMetrics();

	// Notify toolbars about font changing:
	//for (POSITION posTlb = afxAllToolBars.GetHeadPosition(); posTlb != NULL;)
	//{
	//	CMFCToolBar* pToolBar = (CMFCToolBar*) afxAllToolBars.GetNext(posTlb);
	//	ENSURE(pToolBar != NULL);

	//	if (CWnd::FromHandlePermanent(pToolBar->m_hWnd) != NULL)
	//	{
	//		ASSERT_VALID(pToolBar);
	//		pToolBar->OnGlobalFontsChanged();
	//	}
	//}
}

static BOOL CALLBACK InfoEnumProc( HMONITOR hMonitor, HDC /*hdcMonitor*/, LPRECT /*lprcMonitor*/, LPARAM dwData)
{
	CRect* pRect = (CRect*) dwData;

	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfo(hMonitor, &mi))
	{
		CRect rectMon = mi.rcWork;

		pRect->left = min(pRect->left, rectMon.left);
		pRect->right = max(pRect->right, rectMon.right);
		pRect->top = min(pRect->top, rectMon.top);
		pRect->bottom = max(pRect->bottom, rectMon.bottom);
	}

	return TRUE;
}

void AFX_GLOBAL_DATA::OnSettingChange()
{
	m_bInSettingChange = TRUE;

	m_sizeSmallIcon.cx = ::GetSystemMetrics(SM_CXSMICON);
	m_sizeSmallIcon.cy = ::GetSystemMetrics(SM_CYSMICON);

	m_rectVirtual.SetRectEmpty();

	if (!EnumDisplayMonitors(NULL, NULL, InfoEnumProc, (LPARAM) &m_rectVirtual))
	{
		::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_rectVirtual, 0);
	}

	// Get system menu animation type:
	m_bMenuAnimation = FALSE;
	m_bMenuFadeEffect = FALSE;

	if (!bIsRemoteSession)
	{
		::SystemParametersInfo(SPI_GETMENUANIMATION, 0, &m_bMenuAnimation, 0);

		if (m_bMenuAnimation)
		{
			::SystemParametersInfo(SPI_GETMENUFADE, 0, &m_bMenuFadeEffect, 0);
		}
	}

	m_nShellAutohideBars = 0;
	m_bRefreshAutohideBars = TRUE;

	::SystemParametersInfo(SPI_GETMENUUNDERLINES, 0, &m_bSysUnderlineKeyboardShortcuts, 0);
	m_bUnderlineKeyboardShortcuts = m_bSysUnderlineKeyboardShortcuts;

	m_bInSettingChange = FALSE;
}

void AFX_GLOBAL_DATA::UpdateSysColors()
{
	m_bIsBlackHighContrast = ::GetSysColor(COLOR_3DLIGHT) == RGB(255, 255, 255) && ::GetSysColor(COLOR_3DFACE) == RGB(0, 0, 0);
	m_bIsWhiteHighContrast = ::GetSysColor(COLOR_3DDKSHADOW) == RGB(0, 0, 0) && ::GetSysColor(COLOR_3DFACE) == RGB(255, 255, 255);

	CWindowDC dc(NULL);
	m_nBitsPerPixel = dc.GetDeviceCaps(BITSPIXEL);

	clrBarFace = clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBarShadow = clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clrBarDkShadow = clrBtnDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);
	clrBarLight = clrBtnLight = ::GetSysColor(COLOR_3DLIGHT);
	clrBarHilite = clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrBarText = clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrGrayedText = ::GetSysColor(COLOR_GRAYTEXT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);

	clrHilite = ::GetSysColor(COLOR_HIGHLIGHT);
	clrTextHilite = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

	clrBarWindow = clrWindow = ::GetSysColor(COLOR_WINDOW);
	clrWindowText = ::GetSysColor(COLOR_WINDOWTEXT);

	clrCaptionText = ::GetSysColor(COLOR_CAPTIONTEXT);
	clrMenuText = ::GetSysColor(COLOR_MENUTEXT);

	clrActiveCaption = ::GetSysColor(COLOR_ACTIVECAPTION);
	clrInactiveCaption = ::GetSysColor(COLOR_INACTIVECAPTION);

	clrActiveCaptionGradient = ::GetSysColor(COLOR_GRADIENTACTIVECAPTION);
	clrInactiveCaptionGradient = ::GetSysColor(COLOR_GRADIENTINACTIVECAPTION);

	clrActiveBorder = ::GetSysColor(COLOR_ACTIVEBORDER);
	clrInactiveBorder = ::GetSysColor(COLOR_INACTIVEBORDER);

	clrInactiveCaptionText = ::GetSysColor(COLOR_INACTIVECAPTIONTEXT);

	if (m_bIsBlackHighContrast)
	{
		clrHotLinkNormalText = clrWindowText;
		clrHotLinkHoveredText = clrWindowText;
		clrHotLinkVisitedText = clrWindowText;
	}
	else
	{
		clrHotLinkNormalText = ::GetSysColor(COLOR_HOTLIGHT);
		clrHotLinkHoveredText = RGB(0, 0, 255);   // Bright blue
		clrHotLinkVisitedText = RGB(128, 0, 128); // Violet
	}

	hbrBtnShadow = ::GetSysColorBrush(COLOR_BTNSHADOW);
	ENSURE(hbrBtnShadow != NULL);

	hbrBtnHilite = ::GetSysColorBrush(COLOR_BTNHIGHLIGHT);
	ENSURE(hbrBtnHilite != NULL);

	hbrWindow = ::GetSysColorBrush(COLOR_WINDOW);
	ENSURE(hbrWindow != NULL);

	brBtnFace.DeleteObject();
	brBtnFace.CreateSolidBrush(clrBtnFace);

	brBarFace.DeleteObject();
	brBarFace.CreateSolidBrush(clrBarFace);

	brActiveCaption.DeleteObject();
	brActiveCaption.CreateSolidBrush(clrActiveCaption);

	brInactiveCaption.DeleteObject();
	brInactiveCaption.CreateSolidBrush(clrInactiveCaption);

	brHilite.DeleteObject();
	brHilite.CreateSolidBrush(clrHilite);

	brBlack.DeleteObject();
	brBlack.CreateSolidBrush(clrBtnDkShadow);

	brWindow.DeleteObject();
	brWindow.CreateSolidBrush(clrWindow);

	penHilite.DeleteObject();
	penHilite.CreatePen(PS_SOLID, 1, afxGlobalData.clrHilite);

	penBarFace.DeleteObject();
	penBarFace.CreatePen(PS_SOLID, 1, afxGlobalData.clrBarFace);

	penBarShadow.DeleteObject();
	penBarShadow.CreatePen(PS_SOLID, 1, afxGlobalData.clrBarShadow);

	if (brLight.GetSafeHandle())
	{
		brLight.DeleteObject();
	}

	if (m_nBitsPerPixel > 8)
	{
		COLORREF clrLight = RGB(GetRValue(clrBtnFace) +((GetRValue(clrBtnHilite) - GetRValue(clrBtnFace)) / 2 ),
			GetGValue(clrBtnFace) +((GetGValue(clrBtnHilite) - GetGValue(clrBtnFace)) / 2),
			GetBValue(clrBtnFace) +((GetBValue(clrBtnHilite) - GetBValue(clrBtnFace)) / 2));

		brLight.CreateSolidBrush(clrLight);
	}
	else
	{
		HBITMAP hbmGray = CreateDitherBitmap(dc.GetSafeHdc());
		ENSURE(hbmGray != NULL);

		CBitmap bmp;
		bmp.Attach(hbmGray);

		brLight.CreatePatternBrush(&bmp);
	}

	//CMenuImages::CleanUp();
	//CDockingManager::m_bSDParamsModified = TRUE;
}

BOOL AFX_GLOBAL_DATA::SetMenuFont(LPLOGFONT lpLogFont, BOOL bHorz)
{
	ENSURE(lpLogFont != NULL);

	if (bHorz)
	{
		// Create regular font:
		fontRegular.DeleteObject();
		if (!fontRegular.CreateFontIndirect(lpLogFont))
		{
			ASSERT(FALSE);
			return FALSE;
		}

		// Create underline font:
		lpLogFont->lfUnderline = TRUE;
		fontUnderline.DeleteObject();
		fontUnderline.CreateFontIndirect(lpLogFont);
		lpLogFont->lfUnderline = FALSE;

		// Create bold font(used in the default menu items):
		long lSavedWeight = lpLogFont->lfWeight;
		lpLogFont->lfWeight = 700;

		fontBold.DeleteObject();
		BOOL bResult = fontBold.CreateFontIndirect(lpLogFont);

		lpLogFont->lfWeight = lSavedWeight; // Restore weight

		if (!bResult)
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}
	else // Vertical font
	{
		fontVert.DeleteObject();
		if (!fontVert.CreateFontIndirect(lpLogFont))
		{
			ASSERT(FALSE);
			return FALSE;
		}
	}

	UpdateTextMetrics();
	return TRUE;
}

void AFX_GLOBAL_DATA::UpdateTextMetrics()
{
	CWindowDC dc(NULL);

	CFont* pOldFont = dc.SelectObject(&fontRegular);
	ENSURE(pOldFont != NULL);

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);

	int nExtra = tm.tmHeight < 15 ? 2 : 5;

	m_nTextHeightHorz = tm.tmHeight + nExtra;
	m_nTextWidthHorz = tm.tmMaxCharWidth + nExtra;

	dc.SelectObject(&fontVert);
	dc.GetTextMetrics(&tm);

	nExtra = tm.tmHeight < 15 ? 2 : 5;

	m_nTextHeightVert = tm.tmHeight + nExtra;
	m_nTextWidthVert = tm.tmMaxCharWidth + nExtra;

	dc.SelectObject(pOldFont);
}

HBITMAP AFX_GLOBAL_DATA::CreateDitherBitmap(HDC hDC)
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD      bmiColors[16];
	}
	bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = afxGlobalData.clrBtnFace;

	bmi.bmiColors[0].rgbBlue = GetBValue(clr);
	bmi.bmiColors[0].rgbGreen = GetGValue(clr);
	bmi.bmiColors[0].rgbRed = GetRValue(clr);

	clr = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	bmi.bmiColors[1].rgbBlue = GetBValue(clr);
	bmi.bmiColors[1].rgbGreen = GetGValue(clr);
	bmi.bmiColors[1].rgbRed = GetRValue(clr);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
		patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT, (LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	return hbm;
}

#if (WINVER >= 0x0601)
ITaskbarList* AFX_GLOBAL_DATA::GetITaskbarList()
{
	HRESULT hr = S_OK;

	if (!bIsWindows7 || !m_bTaskBarInterfacesAvailable)
	{
		return NULL;
	}

	if (m_pTaskbarList != NULL)
	{
		return m_pTaskbarList;
	}

	if (!m_bComInitialized)
	{
		hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			m_bComInitialized = TRUE;
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbarList));
	}

	ASSERT(SUCCEEDED(hr));
	return m_pTaskbarList;
}

ITaskbarList3* AFX_GLOBAL_DATA::GetITaskbarList3()
{
	HRESULT hr = S_OK;

	if (!bIsWindows7 || !m_bTaskBarInterfacesAvailable)
	{
		return NULL;
	}

	if (m_pTaskbarList3 != NULL)
	{
		return m_pTaskbarList3;
	}

	if (!m_bComInitialized)
	{
		hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			m_bComInitialized = TRUE;
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pTaskbarList3));
	}

	ASSERT(SUCCEEDED(hr));
	return m_pTaskbarList3;
}

void AFX_GLOBAL_DATA::ReleaseTaskBarRefs()
{
	m_bTaskBarInterfacesAvailable = FALSE;

	if (m_pTaskbarList != NULL)
	{
		RELEASE(m_pTaskbarList);
		m_pTaskbarList = NULL;
	}

	if (m_pTaskbarList3 != NULL)
	{
		RELEASE(m_pTaskbarList3);
		m_pTaskbarList3 = NULL;
	}
}
#endif

void AFX_GLOBAL_DATA::CleanUp()
{
	if (brLight.GetSafeHandle())
	{
		brLight.DeleteObject();
	}

	// cleanup fonts:
	fontRegular.DeleteObject();
	fontBold.DeleteObject();
	fontUnderline.DeleteObject();
	fontVert.DeleteObject();
	fontVertCaption.DeleteObject();
	fontTooltip.DeleteObject();

	ReleaseTaskBarRefs();
	ReleaseD2DRefs();

	if (m_bBufferedPaintInited && m_pfBufferedPaintUnInit != NULL)
	{
		m_pfBufferedPaintUnInit();
		m_bBufferedPaintInited = FALSE;
	}

	if (m_hinstUXThemeDLL != NULL)
	{
		::FreeLibrary(m_hinstUXThemeDLL);
		m_hinstUXThemeDLL = NULL;
	}

	if (m_hinstDwmapiDLL != NULL)
	{
		::FreeLibrary(m_hinstDwmapiDLL);
		m_hinstDwmapiDLL = NULL;
	}

	m_bEnableAccessibility = FALSE;

	if (m_bComInitialized)
	{
		CoUninitialize();
		m_bComInitialized = FALSE;
	}
}

void ControlBarCleanUp()
{
	afxGlobalData.CleanUp();

	//afxMenuHash.CleanUp();

	//CMFCToolBar::CleanUpImages();
	//CMenuImages::CleanUp();

	//if (GetCmdMgr() != NULL)
	//{
	//	GetCmdMgr()->CleanUp();
	//}

	//CKeyboardManager::CleanUp();

	// Destroy visualization manager:
	//CMFCVisualManager::DestroyInstance(TRUE /* bAutoDestroyOnly */);
	//CMFCVisualManagerOffice2007::CleanStyle();
}

#ifdef _AFXDLL
void AfxGlobalsAddRef()
{
	g_dwAfxGlobalDataRef++;
}

void AfxGlobalsRelease()
{
	g_dwAfxGlobalDataRef--;
	if (g_dwAfxGlobalDataRef == 0)
	{
		ControlBarCleanUp();
	}
}
#endif

BOOL AFX_GLOBAL_DATA::DrawParentBackground(CWnd* pWnd, CDC* pDC, LPRECT rectClip)
{
	ASSERT_VALID(pDC);
	ASSERT_VALID(pWnd);

	BOOL bRes = FALSE;

	CRgn rgn;
	if (rectClip != NULL)
	{
		rgn.CreateRectRgnIndirect(rectClip);
		pDC->SelectClipRgn(&rgn);
	}

	CWnd* pParent = pWnd->GetParent();
	ASSERT_VALID(pParent);

	// In Windows XP, we need to call DrawThemeParentBackground function to implement
	// transparent controls
	if (m_pfDrawThemeBackground != NULL)
	{
		bRes = (*m_pfDrawThemeBackground)(pWnd->GetSafeHwnd(), pDC->GetSafeHdc(), rectClip) == S_OK;
	}

	if (!bRes)
	{
		CPoint pt(0, 0);
		pWnd->MapWindowPoints(pParent, &pt, 1);
		pt = pDC->OffsetWindowOrg(pt.x, pt.y);

		bRes = (BOOL) pParent->SendMessage(WM_ERASEBKGND, (WPARAM)pDC->m_hDC);

		pDC->SetWindowOrg(pt.x, pt.y);
	}

	pDC->SelectClipRgn(NULL);

	return bRes;
}

CFrameWnd* AFXGetParentFrame(const CWnd* pWnd)
{
	if (pWnd->GetSafeHwnd() == NULL)
	{
		return NULL;
	}
	ASSERT_VALID(pWnd);

	const CWnd* pParentWnd = pWnd;

	while (pParentWnd != NULL)
	{
		//if (pParentWnd->IsKindOf(RUNTIME_CLASS(CPaneFrameWnd)))
		//{
		//	CPaneFrameWnd* pMiniFrame = DYNAMIC_DOWNCAST(CPaneFrameWnd, pParentWnd);
		//	pParentWnd = pMiniFrame->GetParent();
		//}
		//else
		{
			pParentWnd = pParentWnd->GetParent();
		}

		if (pParentWnd == NULL)
		{
			return NULL;
		}

		if (pParentWnd->IsFrameWnd())
		{
			return(CFrameWnd*)pParentWnd;
		}
	}

	return NULL;
}

COLORREF AFX_GLOBAL_DATA::GetColor(int nColor)
{
	switch(nColor)
	{
		case COLOR_BTNFACE:             return clrBtnFace;
		case COLOR_BTNSHADOW:           return clrBtnShadow;
		case COLOR_3DDKSHADOW:          return clrBtnDkShadow;
		case COLOR_3DLIGHT:             return clrBtnLight;
		case COLOR_BTNHIGHLIGHT:        return clrBtnHilite;
		case COLOR_BTNTEXT:             return clrBtnText;
		case COLOR_GRAYTEXT:            return clrGrayedText;
		case COLOR_WINDOWFRAME:         return clrWindowFrame;

		case COLOR_HIGHLIGHT:           return clrHilite;
		case COLOR_HIGHLIGHTTEXT:       return clrTextHilite;

		case COLOR_WINDOW:              return clrWindow;
		case COLOR_WINDOWTEXT:          return clrWindowText;

		case COLOR_CAPTIONTEXT:         return clrCaptionText;
		case COLOR_MENUTEXT:            return clrMenuText;

		case COLOR_ACTIVECAPTION:       return clrActiveCaption;
		case COLOR_INACTIVECAPTION:     return clrInactiveCaption;

		case COLOR_ACTIVEBORDER:        return clrActiveBorder;
		case COLOR_INACTIVEBORDER:      return clrInactiveBorder;

		case COLOR_INACTIVECAPTIONTEXT: return clrInactiveCaptionText;
	}

	return ::GetSysColor(nColor);
}

BOOL AFX_GLOBAL_DATA::SetLayeredAttrib(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	return(::SetLayeredWindowAttributes(hwnd, crKey, bAlpha, dwFlags));
}

void AFX_GLOBAL_DATA::EnableAccessibilitySupport(BOOL bEnable/* = TRUE*/)
{
	m_bEnableAccessibility = bEnable;
}

CString AFX_GLOBAL_DATA::RegisterWindowClass(LPCTSTR lpszClassNamePrefix)
{
	ENSURE(lpszClassNamePrefix != NULL);

	// Register a new window class:
	HINSTANCE hInst = AfxGetInstanceHandle();
	UINT uiClassStyle = CS_DBLCLKS;
	HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);
	HBRUSH hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

	CString strClassName;
	strClassName.Format(_T("%s:%x:%x:%x:%x"), lpszClassNamePrefix, (UINT_PTR)hInst, uiClassStyle, (UINT_PTR)hCursor, (UINT_PTR)hbrBackground);

	// See if the class already exists:
	WNDCLASS wndcls;
	if (::GetClassInfo(hInst, strClassName, &wndcls))
	{
		// Already registered, assert everything is good:
		ASSERT(wndcls.style == uiClassStyle);
	}
	else
	{
		// Otherwise we need to register a new class:
		wndcls.style = uiClassStyle;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = hCursor;
		wndcls.hbrBackground = hbrBackground;
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = strClassName;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
		}
	}

	return strClassName;
}

BOOL AFX_GLOBAL_DATA::ExcludeTag(CString& strBuffer, LPCTSTR lpszTag, CString& strTag, BOOL bIsCharsList /* = FALSE*/)
{
	const int iBufLen = strBuffer.GetLength();

	CString strTagStart = _T("<");
	strTagStart += lpszTag;
	strTagStart += _T(">");

	const int iTagStartLen = strTagStart.GetLength();

	int iStart = -1;

	int iIndexStart = strBuffer.Find(strTagStart);
	if (iIndexStart < 0)
	{
		return FALSE;
	}

	iStart = iIndexStart + iTagStartLen;

	CString strTagEnd = _T("</");
	strTagEnd += lpszTag;
	strTagEnd += _T('>');

	const int iTagEndLen = strTagEnd.GetLength();

	int iIndexEnd =  -1;
	int nBalanse = 1;
	for (int i = iStart; i < iBufLen - iTagEndLen + 1; i ++)
	{
		if (strBuffer [i] != '<')
		{
			continue;
		}

		if (i < iBufLen - iTagStartLen && _tcsncmp(strBuffer.Mid(i), strTagStart, iTagStartLen) == 0)
		{
			i += iTagStartLen - 1;
			nBalanse ++;
			continue;
		}

		if (_tcsncmp(strBuffer.Mid(i), strTagEnd, iTagEndLen) == 0)
		{
			nBalanse --;
			if (nBalanse == 0)
			{
				iIndexEnd = i;
				break;
			}

			i += iTagEndLen - 1;
		}
	}

	if (iIndexEnd == -1 || iStart > iIndexEnd)
	{
		return FALSE;
	}

	strTag = strBuffer.Mid(iStart, iIndexEnd - iStart);
	strTag.TrimLeft();
	strTag.TrimRight();

	strBuffer.Delete(iIndexStart, iIndexEnd + iTagEndLen - iIndexStart);

	if (bIsCharsList)
	{
		if (strTag.GetLength() > 1 && strTag [0] == _T('\"'))
		{
			strTag = strTag.Mid(1, strTag.GetLength() - 2);
		}

		strTag.Replace(_T("\\t"), _T("\t"));
		strTag.Replace(_T("\\n"), _T("\n"));
		strTag.Replace(_T("\\r"), _T("\r"));
		strTag.Replace(_T("\\b"), _T("\b"));
		strTag.Replace(_T("LT"), _T("<"));
		strTag.Replace(_T("GT"), _T(">"));
		strTag.Replace(_T("AMP"), _T("&"));
	}

	return TRUE;
}

BOOL AFX_GLOBAL_DATA::DwmExtendFrameIntoClientArea(HWND hWnd, AFX_MARGINS* pMargins)
{
	if (m_pfDwmExtendFrameIntoClientArea == NULL)
	{
		return FALSE;
	}

	HRESULT hres = (*m_pfDwmExtendFrameIntoClientArea)(hWnd, pMargins);
	return hres == S_OK;
}

LRESULT AFX_GLOBAL_DATA::DwmDefWindowProc(HWND hWnd, UINT message, WPARAM wp, LPARAM lp)
{
	if (m_pfDwmDefWindowProc == NULL)
	{
		return(LRESULT)-1;
	}

	LRESULT lres = 0;
	(*m_pfDwmDefWindowProc)(hWnd, message, wp, lp, &lres);

	return lres;
}

BOOL AFX_GLOBAL_DATA::DwmIsCompositionEnabled()
{
	if (m_pfDwmIsCompositionEnabled == NULL || bDisableAero)
	{
		return FALSE;
	}

	BOOL bEnabled = FALSE;

	(*m_pfDwmIsCompositionEnabled)(&bEnabled);
	return bEnabled;
}

BOOL AFX_GLOBAL_DATA::DrawTextOnGlass(HTHEME hTheme, CDC* pDC, int iPartId, int iStateId, CString strText, CRect rect, DWORD dwFlags, int nGlowSize, COLORREF clrText)
{
	//---- bits used in dwFlags of DTTOPTS ----
#define AFX_DTT_TEXTCOLOR    (1UL << 0)      // crText has been specified
#define AFX_DTT_BORDERCOLOR  (1UL << 1)      // crBorder has been specified
#define AFX_DTT_SHADOWCOLOR  (1UL << 2)      // crShadow has been specified
#define AFX_DTT_SHADOWTYPE   (1UL << 3)      // iTextShadowType has been specified
#define AFX_DTT_SHADOWOFFSET (1UL << 4)      // ptShadowOffset has been specified
#define AFX_DTT_BORDERSIZE   (1UL << 5)      // nBorderSize has been specified
#define AFX_DTT_FONTPROP     (1UL << 6)      // iFontPropId has been specified
#define AFX_DTT_COLORPROP    (1UL << 7)      // iColorPropId has been specified
#define AFX_DTT_STATEID      (1UL << 8)      // IStateId has been specified
#define AFX_DTT_CALCRECT     (1UL << 9)      // Use pRect as and in/out parameter
#define AFX_DTT_APPLYOVERLAY (1UL << 10)     // fApplyOverlay has been specified
#define AFX_DTT_GLOWSIZE     (1UL << 11)     // iGlowSize has been specified
#define AFX_DTT_CALLBACK     (1UL << 12)     // pfnDrawTextCallback has been specified
#define AFX_DTT_COMPOSITED   (1UL << 13)     // Draws text with antialiased alpha(needs a DIB section)

	if (hTheme == NULL || m_pfDrawThemeTextEx == NULL || !DwmIsCompositionEnabled())
	{
		pDC->DrawText(strText, rect, dwFlags);
		return FALSE;
	}

	CComBSTR bstmp = (LPCTSTR)strText;

	wchar_t* wbuf = new wchar_t[bstmp.Length() + 1];
	wcscpy_s(wbuf, bstmp.Length() + 1, bstmp);

	AFX_DTTOPTS dto;
	memset(&dto, 0, sizeof(AFX_DTTOPTS));
	dto.dwSize = sizeof(AFX_DTTOPTS);
	dto.dwFlags = AFX_DTT_COMPOSITED;

	if (nGlowSize > 0)
	{
		dto.dwFlags |= AFX_DTT_GLOWSIZE;
		dto.iGlowSize = nGlowSize;
	}

	if (clrText != (COLORREF)-1)
	{
		dto.dwFlags |= AFX_DTT_TEXTCOLOR;
		dto.crText = clrText;
	}

	(*m_pfDrawThemeTextEx)(hTheme, pDC->GetSafeHdc(), iPartId, iStateId, wbuf, -1, dwFlags, rect, &dto);

	delete [] wbuf;

	return TRUE;
}

HCURSOR AFX_GLOBAL_DATA::GetHandCursor()
{
	if (m_hcurHand == NULL)
	{
		m_hcurHand = ::LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_HAND));
	}

	return m_hcurHand;
}

BOOL AFX_GLOBAL_DATA::Resume()
{
	m_hinstUXThemeDLL = ::AfxCtxLoadLibraryW(L"UxTheme.dll");

	if (m_hinstUXThemeDLL != NULL)
	{
		m_pfDrawThemeBackground = (DRAWTHEMEPARENTBACKGROUND)::GetProcAddress (m_hinstUXThemeDLL, "DrawThemeParentBackground");
		m_pfDrawThemeTextEx = (DRAWTHEMETEXTEX)::GetProcAddress (m_hinstUXThemeDLL, "DrawThemeTextEx");
		m_pfBeginBufferedPaint = (BEGINBUFFEREDPAINT)::GetProcAddress (m_hinstUXThemeDLL, "BeginBufferedPaint");
		m_pfEndBufferedPaint = (ENDBUFFEREDPAINT)::GetProcAddress (m_hinstUXThemeDLL, "EndBufferedPaint");
	}
	else
	{
		m_pfDrawThemeBackground = NULL;
		m_pfDrawThemeTextEx = NULL;
		m_pfBeginBufferedPaint = NULL;
		m_pfEndBufferedPaint = NULL;
	}

	if (m_hinstDwmapiDLL != NULL)
	{
		m_hinstDwmapiDLL = AfxLoadSystemLibraryUsingFullPath(L"dwmapi.dll");
		ENSURE(m_hinstDwmapiDLL != NULL);

		m_pfDwmExtendFrameIntoClientArea = (DWMEXTENDFRAMEINTOCLIENTAREA)::GetProcAddress (m_hinstDwmapiDLL, "DwmExtendFrameIntoClientArea");
		m_pfDwmDefWindowProc = (DWMDEFWINDOWPROC) ::GetProcAddress (m_hinstDwmapiDLL, "DwmDefWindowProc");
		m_pfDwmIsCompositionEnabled = (DWMISCOMPOSITIONENABLED)::GetProcAddress (m_hinstDwmapiDLL, "DwmIsCompositionEnabled");
	}

	if (m_bEnableAccessibility)
	{
		EnableAccessibilitySupport();
	}

	//CMFCVisualManagerOffice2007::CleanStyle ();

	//if (CMFCVisualManager::m_pRTIDefault != NULL)
	//{
	//	CMFCVisualManager::SetDefaultManager (CMFCVisualManager::m_pRTIDefault);
	//}

	return TRUE;
}

BOOL AFX_GLOBAL_DATA::GetNonClientMetrics (NONCLIENTMETRICS& info)
{
	struct AFX_OLDNONCLIENTMETRICS
	{
		UINT    cbSize;
		int     iBorderWidth;
		int     iScrollWidth;
		int     iScrollHeight;
		int     iCaptionWidth;
		int     iCaptionHeight;
		LOGFONT lfCaptionFont;
		int     iSmCaptionWidth;
		int     iSmCaptionHeight;
		LOGFONT lfSmCaptionFont;
		int     iMenuWidth;
		int     iMenuHeight;
		LOGFONT lfMenuFont;
		LOGFONT lfStatusFont;
		LOGFONT lfMessageFont;
	};

	if (_AfxGetComCtlVersion() < MAKELONG(1, 6))
	{
		info.cbSize = sizeof(AFX_OLDNONCLIENTMETRICS);
	}

	return ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, info.cbSize, &info, 0);
}


BOOL AFXAPI AfxIsExtendedFrameClass(CWnd* pWnd)
{
	ENSURE( pWnd );
	//if (pWnd->IsKindOf(RUNTIME_CLASS(CFrameWndEx)))
	//{
	//	return TRUE;
	//}
	//if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	//{
	//	return TRUE;
	//}
	//if (pWnd->IsKindOf(RUNTIME_CLASS(COleIPFrameWndEx)))
	//{
	//	return TRUE;
	//}
	//if (pWnd->IsKindOf(RUNTIME_CLASS(COleDocIPFrameWndEx)))
	//{
	//	return TRUE;
	//}
	//if (pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWndEx)))
	//{
	//	return TRUE;
	//}
	return FALSE;
}


BOOL AFXAPI AfxIsMFCToolBar(CWnd* pWnd)
{
	ENSURE( pWnd );
	//if (pWnd->IsKindOf(RUNTIME_CLASS(CMFCToolBar)))
	//{
	//	return TRUE;
	//}
	return FALSE;
}

HRESULT AFX_GLOBAL_DATA::ShellCreateItemFromParsingName(PCWSTR pszPath, IBindCtx *pbc, REFIID riid, void **ppv)
{
	static HMODULE hShellDll = AfxCtxLoadLibrary(_T("Shell32.dll"));
	ENSURE(hShellDll != NULL);

	typedef	HRESULT (__stdcall *PFNSHCREATEITEMFROMPARSINGNAME)(
		PCWSTR,
		IBindCtx*,
		REFIID,
		void**
		);

	PFNSHCREATEITEMFROMPARSINGNAME pSHCreateItemFromParsingName =
		(PFNSHCREATEITEMFROMPARSINGNAME)GetProcAddress(hShellDll, "SHCreateItemFromParsingName");
	if (pSHCreateItemFromParsingName == NULL)
	{
		return E_FAIL;
	}

	return (*pSHCreateItemFromParsingName)(pszPath, pbc, riid, ppv);
}

BOOL AFX_GLOBAL_DATA::InitD2D(D2D1_FACTORY_TYPE d2dFactoryType, DWRITE_FACTORY_TYPE writeFactoryType)
{
	if (m_bD2DInitialized)
	{
		return TRUE;
	}

	HRESULT hr = S_OK;

	if (!m_bComInitialized)
	{
		hr = CoInitialize(NULL);
		if (FAILED(hr))
		{
			return FALSE;
		}
	}

	m_bComInitialized = TRUE;

	if ((m_hinstD2DDLL = AfxLoadSystemLibraryUsingFullPath(L"D2D1.dll")) == NULL)
	{
		return FALSE;
	}

	typedef HRESULT (WINAPI * D2D1CREATEFACTORY)(D2D1_FACTORY_TYPE factoryType, REFIID riid, CONST D2D1_FACTORY_OPTIONS *pFactoryOptions, void **ppIFactory);
	typedef HRESULT (WINAPI * DWRITECREATEFACTORY)(DWRITE_FACTORY_TYPE factoryType, REFIID riid, IUnknown **factory);

	D2D1CREATEFACTORY pfD2D1CreateFactory = (D2D1CREATEFACTORY)::GetProcAddress(m_hinstD2DDLL, "D2D1CreateFactory");
	if (pfD2D1CreateFactory != NULL)
	{
		hr = (*pfD2D1CreateFactory)(d2dFactoryType, __uuidof(ID2D1Factory),
			NULL, reinterpret_cast<void **>(&m_pDirect2dFactory));
		if (FAILED(hr))
		{
			m_pDirect2dFactory = NULL;
			return FALSE;
		}
	}

	m_pfD2D1MakeRotateMatrix = (D2D1MAKEROTATEMATRIX)::GetProcAddress(m_hinstD2DDLL, "D2D1MakeRotateMatrix");

	m_hinstDWriteDLL = AfxLoadSystemLibraryUsingFullPath(L"DWrite.dll");
	if (m_hinstDWriteDLL != NULL)
	{
		DWRITECREATEFACTORY pfD2D1CreateFactory = (DWRITECREATEFACTORY)::GetProcAddress(m_hinstDWriteDLL, "DWriteCreateFactory");
		if (pfD2D1CreateFactory != NULL)
		{
			hr = (*pfD2D1CreateFactory)(writeFactoryType, __uuidof(IDWriteFactory), (IUnknown**)&m_pWriteFactory);
		}
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&m_pWicFactory);

	m_bD2DInitialized = TRUE;
	return TRUE;
}

void AFX_GLOBAL_DATA::ReleaseD2DRefs()
{
	if (!m_bD2DInitialized)
	{
		return;
	}

	if (m_pDirect2dFactory != NULL)
	{
		m_pDirect2dFactory->Release();
		m_pDirect2dFactory = NULL;
	}

	if (m_pWriteFactory != NULL)
	{
		m_pWriteFactory->Release();
		m_pWriteFactory = NULL;
	}

	if (m_pWicFactory != NULL)
	{
		m_pWicFactory->Release();
		m_pWicFactory = NULL;
	}

	if (m_hinstD2DDLL != NULL)
	{
		::FreeLibrary(m_hinstD2DDLL);
	}

	if (m_hinstDWriteDLL != NULL)
	{
		::FreeLibrary(m_hinstDWriteDLL);
	}

	m_bD2DInitialized = FALSE;
}

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//
// CWnd support for MFC Control containment (Feature Pack controls)
//

////////////////////////////////////////////////////////////////////////////

static void DoRegisterWindowClass(LPCTSTR lpszClassName, LPCTSTR lpszBaseClassName)
{
	ASSERT(lpszClassName != NULL);
	ASSERT(lpszBaseClassName != NULL);

	WNDCLASS wnd = {0};

	HINSTANCE hInst = AfxGetInstanceHandle();
	if (!AfxCtxGetClassInfo(hInst, lpszBaseClassName, &wnd))
	{
		wnd.style = CS_DBLCLKS;
		wnd.hInstance = hInst;
		wnd.lpfnWndProc = ::DefWindowProc;
	}

	wnd.lpszClassName = lpszClassName;
	AfxRegisterClass(&wnd);
}

void AfxRegisterMFCCtrlClasses()
{
	DoRegisterWindowClass(_T("MFCButton"), WC_BUTTON);
	DoRegisterWindowClass(_T("MFCColorButton"), WC_BUTTON);
	DoRegisterWindowClass(_T("MFCEditBrowse"), WC_EDIT);
	DoRegisterWindowClass(_T("MFCFontComboBox"), WC_COMBOBOX);
	DoRegisterWindowClass(_T("MFCLink"), WC_BUTTON);
	DoRegisterWindowClass(_T("MFCMaskedEdit"), WC_EDIT);
	DoRegisterWindowClass(_T("MFCMenuButton"), WC_BUTTON);
	DoRegisterWindowClass(_T("MFCPropertyGrid"), WC_STATIC);
	DoRegisterWindowClass(_T("MFCShellList"), WC_LISTVIEW);
	DoRegisterWindowClass(_T("MFCShellTree"), WC_TREEVIEW);
	DoRegisterWindowClass(_T("MFCVSListBox"), WC_STATIC);
}

////////////////////////////////////////////////////////////////////////////
// CMFCControlContainer

CMFCControlContainer::CMFCControlContainer(CWnd* pWnd) : m_pWnd(pWnd)
{
}

CMFCControlContainer::~CMFCControlContainer()
{
	FreeSubclassedControls();
	ClearControlData();
}

BOOL CMFCControlContainer::SubclassDlgControls()
{
	if (m_pWnd->GetSafeHwnd() != NULL)
	{
		// Subclass Feature Pack controls:
		CWnd* pWndChild = m_pWnd->GetWindow(GW_CHILD);
		while (pWndChild != NULL)
		{
			ASSERT_VALID(pWndChild);

			TCHAR lpszClassName [MAX_CLASS_NAME + 1];

			::GetClassName(pWndChild->GetSafeHwnd(), lpszClassName, MAX_CLASS_NAME);
			CWnd* pWndSubclassedCtrl = CreateDlgControl(lpszClassName);

			if (pWndSubclassedCtrl != NULL)
			{
				m_arSubclassedCtrls.Add((CObject*)pWndSubclassedCtrl);
				pWndSubclassedCtrl->SubclassWindow(pWndChild->GetSafeHwnd());
			}

			pWndChild = pWndChild->GetNextWindow();
		}

		return TRUE;
	}

	return FALSE;
}

void CMFCControlContainer::FreeSubclassedControls()
{
	// Free subclassed controls: 
	for (int i = 0; i < m_arSubclassedCtrls.GetCount(); i++)
	{
		if (m_arSubclassedCtrls [i] != NULL)
		{
			delete m_arSubclassedCtrls [i];
		}
	}
	m_arSubclassedCtrls.RemoveAll();
}

CWnd* CMFCControlContainer::CreateDlgControl(LPCTSTR /*lpszClassName*/)
{
	ASSERT(m_pWnd->GetSafeHwnd() != NULL);

	//if (lpszClassName != NULL)
	//{
	//	CString strClass = lpszClassName;
	//	CWnd* pWndSubclassedCtrl = NULL;

	//	if (strClass == _T("MFCButton"))
	//	{
	//		pWndSubclassedCtrl = new CMFCButton;
	//	}
	//	else if (strClass == _T("MFCColorButton"))
	//	{
	//		pWndSubclassedCtrl = new CMFCColorButton;
	//	}
	//	else if (strClass == _T("MFCEditBrowse"))
	//	{
	//		pWndSubclassedCtrl = new CMFCEditBrowseCtrl;
	//	}
	//	else if (strClass == _T("MFCFontComboBox"))
	//	{
	//		pWndSubclassedCtrl = new CMFCFontComboBox;
	//	}
	//	else if (strClass == _T("MFCLink"))
	//	{
	//		pWndSubclassedCtrl = new CMFCLinkCtrl;
	//	}
	//	else if (strClass == _T("MFCMaskedEdit"))
	//	{
	//		pWndSubclassedCtrl = new CMFCMaskedEdit;
	//	}
	//	else if (strClass == _T("MFCMenuButton"))
	//	{
	//		pWndSubclassedCtrl = new CMFCMenuButton;
	//	}
	//	else if (strClass == _T("MFCPropertyGrid"))
	//	{
	//		pWndSubclassedCtrl = new CMFCPropertyGridCtrl;
	//	}
	//	else if (strClass == _T("MFCShellList"))
	//	{
	//		pWndSubclassedCtrl = new CMFCShellListCtrl;
	//	}
	//	else if (strClass == _T("MFCShellTree"))
	//	{
	//		pWndSubclassedCtrl = new CMFCShellTreeCtrl;
	//	}
	//	else if (strClass == _T("MFCVSListBox"))
	//	{
	//		pWndSubclassedCtrl = new CVSListBox;
	//	}

	//	return pWndSubclassedCtrl;
	//}

	return NULL;
}

BOOL CMFCControlContainer::IsSubclassedFeaturePackControl(HWND hWndCtrl)
{
	if (hWndCtrl == NULL)
	{
		return FALSE;
	}

	for (int i = 0; i < m_arSubclassedCtrls.GetCount(); i++)
	{
		CWnd* pWnd = (CWnd*)m_arSubclassedCtrls[i];
		if (pWnd->GetSafeHwnd() == hWndCtrl)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CMFCControlContainer::PreUnsubclassControl(CWnd* /*pControl*/)
{
	//CMFCShellListCtrl* pListCtrl = DYNAMIC_DOWNCAST(CMFCShellListCtrl, pControl);
	//if (pListCtrl != NULL && pListCtrl->GetHeaderCtrl().GetSafeHwnd() != NULL)
	//{
	//	pListCtrl->GetHeaderCtrl().UnsubclassWindow();
	//}
}

BOOL CMFCControlContainer::ReSubclassControl(HWND hWndCtrl, WORD nIDC, CWnd& control)
{
	if (hWndCtrl == NULL)
	{
		return FALSE;
	}

	int nIndex = -1;
	for (int i = 0; i < m_arSubclassedCtrls.GetCount(); i++)
	{
		CWnd* pWnd = (CWnd*)m_arSubclassedCtrls [i];
		if (pWnd->GetSafeHwnd() == hWndCtrl)
		{
			nIndex = i;
			break;
		}
	}

	if (nIndex != -1)
	{
		CWnd* pWnd = DYNAMIC_DOWNCAST(CWnd, m_arSubclassedCtrls [nIndex]);
		
		if (pWnd->GetSafeHwnd() != NULL)
		{
			ASSERT_VALID(pWnd);

			// get init state
			DWORD dwSize = 0;
			BYTE* pbInitData = NULL;
			GetControlData(nIDC, dwSize, pbInitData);

			// Free old subclassed control:
			m_arSubclassedCtrls [nIndex] = NULL;

			// unsubclass
			PreUnsubclassControl(pWnd);
			VERIFY(pWnd->UnsubclassWindow() == hWndCtrl);
			// destroy
			delete pWnd;

			// subclass
			if (!control.SubclassWindow(hWndCtrl))
			{
				ASSERT(FALSE);      // possibly trying to subclass twice?
				AfxThrowNotSupportedException();
			}

			// set init state
			if (dwSize > 0)
			{
				control.SendMessage(WM_MFC_INITCTRL, (WPARAM)dwSize, (LPARAM)pbInitData);
			}

			return TRUE;
		}
	}

	return FALSE;
}

void CMFCControlContainer::SetControlData(WORD nIDC, DWORD dwSize, BYTE* pbData)
{
	CByteArray* pArray = new CByteArray;
	pArray->SetSize(dwSize);

	BYTE* pbBuffer = pArray->GetData();
	if (memcpy_s(pbBuffer, dwSize, pbData, dwSize) != 0)
	{
		delete pArray;
		ASSERT(FALSE);
		return;
	}

	m_mapControlData.SetAt(nIDC, pArray);
}

BOOL CMFCControlContainer::GetControlData(WORD nIDC, DWORD& dwSize, BYTE*& pbData)
{
	CObject* pData = NULL;
	if (m_mapControlData.Lookup(nIDC, pData) && pData != NULL)
	{
		CByteArray* pArray = (CByteArray*)pData;
		dwSize = (DWORD)pArray->GetSize();
		pbData = pArray->GetData();
		return TRUE;
	}

	return FALSE;
}

void CMFCControlContainer::ClearControlData()
{
	WORD nIDC;
	CObject* pData = NULL;
	POSITION pos = m_mapControlData.GetStartPosition();
	while (pos != NULL)
	{
		m_mapControlData.GetNextAssoc(pos, nIDC, pData); 
		CByteArray* pArray = (CByteArray*)pData; 
		delete pArray;
	}

	m_mapControlData.RemoveAll();
}

////////////////////////////////////////////////////////////////////////////
// Accessing dialog DLGINIT helpers

int __stdcall CMFCControlContainer::UTF8ToString(LPCSTR lpSrc, CString& strDst, int nLength)
{
	LPTSTR lpDst = NULL;
	int count = ::MultiByteToWideChar(CP_UTF8, 0, lpSrc, nLength, NULL, 0);
	if (count <= 0)
	{
		return 0;
	}

	LPWSTR lpWide = new WCHAR[count + 1];
	memset(lpWide, 0, (count + 1) * sizeof(WCHAR));

	::MultiByteToWideChar(CP_UTF8, 0, lpSrc, nLength, lpWide, count);

#ifdef _UNICODE
	lpDst = lpWide;
#else
	count = ::WideCharToMultiByte(::GetACP(), 0, lpWide, -1, NULL, 0, NULL, 0);

	if (count > 0)
	{
		lpDst = new char[count + 1];
		memset(lpDst, 0, count + 1);

		::WideCharToMultiByte(::GetACP(), 0, lpWide, -1, lpDst, count, NULL, 0);
	}

	delete [] lpWide;
#endif

	strDst = lpDst;
	delete[] lpDst;
	return count;
}

BOOL __stdcall CMFCControlContainer::ReadBoolProp(CTagManager& /*tagManager*/, LPCTSTR lpszTag, BOOL& bMember)
{
	if (lpszTag == NULL)
	{
		return FALSE;
	}

	CString str;
	//tagManager.ExcludeTag(lpszTag, str);

	if (str.IsEmpty())
	{
		return FALSE;
	}

	bMember = (str.CompareNoCase(PS_True) == 0);
	return TRUE;
}

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// CMDIFrameWnd

BEGIN_MESSAGE_MAP(CMDIFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CMDIFrameWnd)
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, CMDIFrameWnd::OnIdleUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_ARRANGE, &CMDIFrameWnd::OnUpdateMDIWindowCmd)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_CASCADE, &CMDIFrameWnd::OnUpdateMDIWindowCmd)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_HORZ, &CMDIFrameWnd::OnUpdateMDIWindowCmd)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_TILE_VERT, &CMDIFrameWnd::OnUpdateMDIWindowCmd)
	ON_WM_SIZE()
	ON_COMMAND_EX(ID_WINDOW_ARRANGE, &CMDIFrameWnd::OnMDIWindowCmd)
	ON_COMMAND_EX(ID_WINDOW_CASCADE, &CMDIFrameWnd::OnMDIWindowCmd)
	ON_COMMAND_EX(ID_WINDOW_TILE_HORZ, &CMDIFrameWnd::OnMDIWindowCmd)
	ON_COMMAND_EX(ID_WINDOW_TILE_VERT, &CMDIFrameWnd::OnMDIWindowCmd)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEW, &CMDIFrameWnd::OnUpdateMDIWindowCmd)
	ON_COMMAND(ID_WINDOW_NEW, &CMDIFrameWnd::OnWindowNew)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_COMMANDHELP, &CMDIFrameWnd::OnCommandHelp)
	ON_WM_MENUCHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMDIFrameWnd::CMDIFrameWnd()
{
	m_hWndMDIClient = NULL;
}

BOOL CMDIFrameWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// send to MDI child first - will be re-sent through OnCmdMsg later
	CMDIChildWnd* pActiveChild = MDIGetActive();
	if (pActiveChild != NULL && AfxCallWndProc(pActiveChild,
	  pActiveChild->m_hWnd, WM_COMMAND, wParam, lParam) != 0)
		return TRUE; // handled by child

	if (CFrameWnd::OnCommand(wParam, lParam))
		return TRUE; // handled through normal mechanism (MDI child or frame)

	HWND hWndCtrl = (HWND)lParam;

	ASSERT(AFX_IDM_FIRST_MDICHILD == 0xFF00);
	if (hWndCtrl == NULL && (LOWORD(wParam) & 0xf000) == 0xf000)
	{
		// menu or accelerator within range of MDI children
		// default frame proc will handle it
		DefWindowProc(WM_COMMAND, wParam, lParam);
		return TRUE;
	}

	return FALSE;   // not handled
}

BOOL CMDIFrameWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	// pump through active child FIRST
	if (pActiveChild != NULL)
	{
		CPushRoutingFrame push(this);
		if (pActiveChild->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
			return TRUE;
	}

	// then pump through normal frame
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CMDIFrameWnd::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 0 && IsTracking())
		lParam = HID_BASE_COMMAND+m_nIDTracking;

	CMDIChildWnd* pActiveChild = MDIGetActive();
	if (pActiveChild != NULL && AfxCallWndProc(pActiveChild,
	  pActiveChild->m_hWnd, WM_COMMANDHELP, wParam, lParam) != 0)
	{
		// handled by child
		return TRUE;
	}

	if (CFrameWnd::OnCommandHelp(wParam, lParam))
	{
		// handled by our base
		return TRUE;
	}

	if (lParam != 0)
	{
		CWinApp* pApp = AfxGetApp();
		if (pApp != NULL)
		{
			AfxGetApp()->WinHelpInternal(lParam);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMDIFrameWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext*)
{
	CMenu* pMenu = NULL;
	if (m_hMenuDefault == NULL)
	{
		// default implementation for MFC V1 backward compatibility
		pMenu = GetMenu();
		ASSERT(pMenu != NULL);
		// This is attempting to guess which sub-menu is the Window menu.
		// The Windows user interface guidelines say that the right-most
		// menu on the menu bar should be Help and Window should be one
		// to the left of that.
		int iMenu = pMenu->GetMenuItemCount() - 2;

		// If this assertion fails, your menu bar does not follow the guidelines
		// so you will have to override this function and call CreateClient
		// appropriately or use the MFC V2 MDI functionality.
		ASSERT(iMenu >= 0);
		pMenu = pMenu->GetSubMenu(iMenu);
		ASSERT(pMenu != NULL);
	}

	return CreateClient(lpcs, pMenu);
}

BOOL CMDIFrameWnd::CreateClient(LPCREATESTRUCT lpCreateStruct,
	CMenu* pWindowMenu)
{
	ASSERT(m_hWnd != NULL);
	ASSERT(m_hWndMDIClient == NULL);
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER |
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
		MDIS_ALLCHILDSTYLES;    // allow children to be created invisible
	DWORD dwExStyle = 0;
	// will be inset by the frame

	// special styles for 3d effect on Win4
	dwStyle &= ~WS_BORDER;
	dwExStyle = WS_EX_CLIENTEDGE;

	CLIENTCREATESTRUCT ccs;
	ccs.hWindowMenu = pWindowMenu->GetSafeHmenu();
		// set hWindowMenu for MFC V1 backward compatibility
		// for MFC V2, window menu will be set in OnMDIActivate
	ccs.idFirstChild = AFX_IDM_FIRST_MDICHILD;

	if (lpCreateStruct->style & (WS_HSCROLL|WS_VSCROLL))
	{
		// parent MDIFrame's scroll styles move to the MDICLIENT
		dwStyle |= (lpCreateStruct->style & (WS_HSCROLL|WS_VSCROLL));

		// fast way to turn off the scrollbar bits (without a resize)
		ModifyStyle(WS_HSCROLL|WS_VSCROLL, 0, SWP_NOREDRAW|SWP_FRAMECHANGED);
	}

	// Create MDICLIENT control with special IDC
	if ((m_hWndMDIClient = ::AfxCtxCreateWindowEx(dwExStyle, _T("mdiclient"), NULL,
		dwStyle, 0, 0, 0, 0, m_hWnd, (HMENU)AFX_IDW_PANE_FIRST,
		AfxGetInstanceHandle(), (LPVOID)&ccs)) == NULL)
	{
		TRACE(traceAppMsg, 0, _T("Warning: CMDIFrameWnd::OnCreateClient: failed to create MDICLIENT.")
			_T(" GetLastError returns 0x%8.8X\n"), ::GetLastError());
		return FALSE;
	}
	// Move it to the top of z-order
	::BringWindowToTop(m_hWndMDIClient);

	return TRUE;
}

LRESULT CMDIFrameWnd::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefFrameProc(m_hWnd, m_hWndMDIClient, nMsg, wParam, lParam);
}

BOOL CMDIFrameWnd::PreTranslateMessage(MSG* pMsg)
{
	// check for special cancel modes for ComboBoxes
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_NCLBUTTONDOWN)
		AfxCancelModes(pMsg->hwnd);    // filter clicks

	// allow tooltip messages to be filtered
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;

#ifndef _AFX_NO_OLE_SUPPORT
	// allow hook to consume message
	if (m_pNotifyHook != NULL && m_pNotifyHook->OnPreTranslateMessage(pMsg))
		return TRUE;
#endif

	CMDIChildWnd* pActiveChild = MDIGetActive();

	// current active child gets first crack at it
	if (pActiveChild != NULL && pActiveChild->PreTranslateMessage(pMsg))
		return TRUE;

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		// translate accelerators for frame and any children
		if (m_hAccelTable != NULL &&
			::TranslateAccelerator(m_hWnd, m_hAccelTable, pMsg))
		{
			return TRUE;
		}

		// special processing for MDI accelerators last
		// and only if it is not in SDI mode (print preview)
		if (GetActiveView() == NULL)
		{
			if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
			{
				// the MDICLIENT window may translate it
				if (::TranslateMDISysAccel(m_hWndMDIClient, pMsg))
					return TRUE;
			}
		}
	}

	return FALSE;
}

void CMDIFrameWnd::DelayUpdateFrameMenu(HMENU hMenuAlt)
{
	OnUpdateFrameMenu(hMenuAlt);

	m_nIdleFlags |= idleMenu;
}

void CMDIFrameWnd::OnIdleUpdateCmdUI()
{
	if (m_nIdleFlags & idleMenu)
	{
		DrawMenuBar();
		m_nIdleFlags &= ~idleMenu;
	}
	CFrameWnd::OnIdleUpdateCmdUI();
}

CFrameWnd* CMDIFrameWnd::GetActiveFrame()
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	if (pActiveChild == NULL)
		return this;
	return pActiveChild;
}

BOOL CMDIFrameWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if (cs.lpszClass == NULL)
	{
		VERIFY(AfxDeferRegisterClass(AFX_WNDMDIFRAME_REG));
		cs.lpszClass = _afxWndMDIFrame;
	}
	return TRUE;
}

BOOL CMDIFrameWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
	CWnd* pParentWnd, CCreateContext* pContext)
{
	if (!CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle,
	  pParentWnd, pContext))
		return FALSE;

	// save menu to use when no active MDI child window is present
	ASSERT(m_hWnd != NULL);
	m_hMenuDefault = ::GetMenu(m_hWnd);
	return TRUE;
}

void CMDIFrameWnd::OnDestroy()
{
	CFrameWnd::OnDestroy();     // exit and misc cleanup

	// owned menu stored in shared slot for MDIFRAME
	if (m_hMenuDefault != NULL && ::GetMenu(m_hWnd) != m_hMenuDefault)
	{
		// must go through MDI client to get rid of MDI menu additions
		::SendMessage(m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)m_hMenuDefault, NULL);
		ASSERT(::GetMenu(m_hWnd) == m_hMenuDefault);
	}
}

void CMDIFrameWnd::OnSize(UINT nType, int, int)
{
	// do not call default - it will reposition the MDICLIENT
	if (nType != SIZE_MINIMIZED)
		RecalcLayout();
}

LRESULT CMDIFrameWnd::OnMenuChar(UINT nChar, UINT, CMenu*)
{
	// do not call Default() for Alt+(-) when in print preview mode
	if (m_lpfnCloseProc != NULL && nChar == (UINT)'-')
		return 0;
	else
		return Default();
}

CMDIChildWnd* CMDIFrameWnd::MDIGetActive(BOOL* pbMaximized) const
{
	// check first for MDI client window not created
	if (m_hWndMDIClient == NULL)
	{
		if (pbMaximized != NULL)
			*pbMaximized = FALSE;
		return NULL;
	}

	// MDI client has been created, get active MDI child
	HWND hWnd = (HWND)::SendMessage(m_hWndMDIClient, WM_MDIGETACTIVE, 0,
		(LPARAM)pbMaximized);
	CMDIChildWnd* pWnd = (CMDIChildWnd*)CWnd::FromHandlePermanent(hWnd);
	ASSERT(pWnd == NULL || pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)));

	// check for special pseudo-inactive state
	if (pWnd != NULL && pWnd->m_bPseudoInactive &&
		(pWnd->GetStyle() & WS_VISIBLE) == 0)
	{
		// Window is hidden, active, but m_bPseudoInactive -- return NULL
		pWnd = NULL;
		// Ignore maximized flag if pseudo-inactive and maximized
		if (pbMaximized != NULL)
			*pbMaximized = FALSE;
	}
	return pWnd;
}


CMDIChildWnd* CMDIFrameWnd::CreateNewChild(CRuntimeClass* pClass,
		UINT nResources, HMENU hMenu /* = NULL */, HACCEL hAccel /* = NULL */)
{
	ASSERT(pClass != NULL);
	CMDIChildWnd* pFrame = (CMDIChildWnd*) pClass->CreateObject();
	ASSERT_KINDOF(CMDIChildWnd, pFrame);

	// load the frame
	CCreateContext context;
	context.m_pCurrentFrame = this;

	pFrame->SetHandles(hMenu, hAccel);
	if (!pFrame->LoadFrame(nResources,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, &context))
	{
		TRACE(traceAppMsg, 0, "Couldn't load frame window.\n");
		return NULL;
	}

	CString strFullString, strTitle;
	if (strFullString.LoadString(nResources))
		AfxExtractSubString(strTitle, strFullString, CDocTemplate::docName);

	// redraw the frame and parent
	pFrame->SetTitle(strTitle);
	pFrame->InitialUpdateFrame(NULL, TRUE);

	return pFrame;
}

/////////////////////////////////////////////////////////////////////////////
// CMDIFrameWnd Diagnostics

#ifdef _DEBUG
void CMDIFrameWnd::AssertValid() const
{
	CFrameWnd::AssertValid();
	ASSERT(m_hWndMDIClient == NULL || ::IsWindow(m_hWndMDIClient));
	ASSERT(m_hMenuDefault == NULL || ::IsMenu(m_hMenuDefault));
}

void CMDIFrameWnd::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);

	dc << "m_hWndMDIClient = " << (void*)m_hWndMDIClient;
	dc << "\nm_hMenuDefault = " << (void*)m_hMenuDefault;

	dc << "\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWnd

BEGIN_MESSAGE_MAP(CMDIChildWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CMDIChildWnd)
	ON_WM_MOUSEACTIVATE()
	ON_WM_NCACTIVATE()
	ON_WM_MDIACTIVATE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_NCCREATE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &CMDIChildWnd::OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, &CMDIChildWnd::OnToolTipText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMDIChildWnd::CMDIChildWnd()
{
	m_hMenuShared = NULL;
	m_bPseudoInactive = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWnd special processing

LRESULT CMDIChildWnd::DefWindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return ::DefMDIChildProc(m_hWnd, nMsg, wParam, lParam);
}

BOOL CMDIChildWnd::DestroyWindow()
{
	if (m_hWnd == NULL)
		return FALSE;

	// avoid changing the caption during the destroy message(s)
	CMDIFrameWnd* pFrameWnd = GetMDIFrame();
	HWND hWndFrame = pFrameWnd->m_hWnd;
	ASSERT(::IsWindow(hWndFrame));
	DWORD dwStyle = SetWindowLong(hWndFrame, GWL_STYLE,
		GetWindowLong(hWndFrame, GWL_STYLE) & ~FWS_ADDTOTITLE);

	MDIDestroy();

	if (::IsWindow(hWndFrame))
	{
		ASSERT(hWndFrame == pFrameWnd->m_hWnd);
		SetWindowLong(hWndFrame, GWL_STYLE, dwStyle);
		pFrameWnd->OnUpdateFrameTitle(TRUE);
	}

	return TRUE;
}

BOOL CMDIChildWnd::PreTranslateMessage(MSG* pMsg)
{
	// check for special cancel modes for combo boxes
	if (pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_NCLBUTTONDOWN)
		AfxCancelModes(pMsg->hwnd);    // filter clicks

	// allow tooltip messages to be filtered
	if (CWnd::PreTranslateMessage(pMsg))
		return TRUE;

	// we can't call 'CFrameWnd::PreTranslate' since it will translate
	//  accelerators in the context of the MDI Child - but since MDI Child
	//  windows don't have menus this doesn't work properly.  MDI Child
	//  accelerators must be translated in context of their MDI Frame.

	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		// use document specific accelerator table over m_hAccelTable
		HACCEL hAccel = GetDefaultAccelerator();
		return hAccel != NULL &&
		   ::TranslateAccelerator(GetMDIFrame()->m_hWnd, hAccel, pMsg);
	}
	return FALSE;
}

BOOL CMDIChildWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	ASSERT(cs.style & WS_CHILD);
		// MFC V2 requires that MDI Children are created with proper styles,
		//  usually: WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW.
		// See Technical note TN019 for more details on MFC V1->V2 migration.

	return CFrameWnd::PreCreateWindow(cs);
}

BOOL CMDIChildWnd::Create(LPCTSTR lpszClassName,
	LPCTSTR lpszWindowName, DWORD dwStyle,
	const RECT& rect, CMDIFrameWnd* pParentWnd,
	CCreateContext* pContext)
{
	if (pParentWnd == NULL)
	{
		CWinThread *pThread = AfxGetThread();
		ENSURE_VALID(pThread);
		CWnd* pMainWnd = pThread->m_pMainWnd;
		ENSURE_VALID(pMainWnd);
		ASSERT_KINDOF(CMDIFrameWnd, pMainWnd);
		pParentWnd = (CMDIFrameWnd*)pMainWnd;
	}
	ASSERT(::IsWindow(pParentWnd->m_hWndMDIClient));

	// insure correct window positioning
	pParentWnd->RecalcLayout();

	// first copy into a CREATESTRUCT for PreCreate
	CREATESTRUCT cs;
	cs.dwExStyle = 0L;
	cs.lpszClass = lpszClassName;
	cs.lpszName = lpszWindowName;
	cs.style = dwStyle;
	cs.x = rect.left;
	cs.y = rect.top;
	cs.cx = rect.right - rect.left;
	cs.cy = rect.bottom - rect.top;
	cs.hwndParent = pParentWnd->m_hWnd;
	cs.hMenu = NULL;
	cs.hInstance = AfxGetInstanceHandle();
	cs.lpCreateParams = (LPVOID)pContext;

	if (!PreCreateWindow(cs))
	{
		PostNcDestroy();
		return FALSE;
	}
	// extended style must be zero for MDI Children (except under Win4)
	ASSERT(cs.hwndParent == pParentWnd->m_hWnd);    // must not change

	// now copy into a MDICREATESTRUCT for real create
	MDICREATESTRUCT mcs;
	mcs.szClass = cs.lpszClass;
	mcs.szTitle = cs.lpszName;
	mcs.hOwner = cs.hInstance;
	mcs.x = cs.x;
	mcs.y = cs.y;
	mcs.cx = cs.cx;
	mcs.cy = cs.cy;
	mcs.style = cs.style & ~(WS_MAXIMIZE | WS_VISIBLE);
	mcs.lParam = (LPARAM)cs.lpCreateParams;

	// create the window through the MDICLIENT window
	AfxHookWindowCreate(this);
	HWND hWnd = (HWND)::SendMessage(pParentWnd->m_hWndMDIClient,
		WM_MDICREATE, 0, (LPARAM)&mcs);
	if (!AfxUnhookWindowCreate())
		PostNcDestroy();        // cleanup if MDICREATE fails too soon

	if (hWnd == NULL)
		return FALSE;

	// special handling of visibility (always created invisible)
	if (cs.style & WS_VISIBLE)
	{
		// place the window on top in z-order before showing it
		::BringWindowToTop(hWnd);

		// show it as specified
		if (cs.style & WS_MINIMIZE)
			ShowWindow(SW_SHOWMINIMIZED);
		else if (cs.style & WS_MAXIMIZE)
			ShowWindow(SW_SHOWMAXIMIZED);
		else
			ShowWindow(SW_SHOWNORMAL);

		// make sure it is active (visibility == activation)
		pParentWnd->MDIActivate(this);

		// refresh MDI Window menu
		::SendMessage(pParentWnd->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
	}

	ASSERT(hWnd == m_hWnd);
	return TRUE;
}

BOOL CMDIChildWnd::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
		CWnd* pParentWnd, CCreateContext* pContext)
{
	// only do this once
	ASSERT_VALID_IDR(nIDResource);
	ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

	m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

	// parent must be MDI Frame (or NULL for default)
	ASSERT(pParentWnd == NULL || pParentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	// will be a child of MDIClient
	ASSERT(!(dwDefaultStyle & WS_POPUP));
	dwDefaultStyle |= WS_CHILD;

	// if available - get MDI child menus from doc template
	CMultiDocTemplate* pTemplate;
	if (pContext != NULL &&
		(pTemplate = (CMultiDocTemplate*)pContext->m_pNewDocTemplate) != NULL)
	{
		ASSERT_KINDOF(CMultiDocTemplate, pTemplate);
		// get shared menu from doc template
		m_hMenuShared = pTemplate->m_hMenuShared;
		m_hAccelTable = pTemplate->m_hAccelTable;
	}
	else
	{
		TRACE(traceAppMsg, 0, "Warning: no shared menu/acceltable for MDI Child window.\n");
			// if this happens, programmer must load these manually
	}

	CString strFullString, strTitle;
	if (strFullString.LoadString(nIDResource))
		AfxExtractSubString(strTitle, strFullString, 0);    // first sub-string

	ASSERT(m_hWnd == NULL);
	if (!Create(GetIconWndClass(dwDefaultStyle, nIDResource),
	  strTitle, dwDefaultStyle, rectDefault,
	  (CMDIFrameWnd*)pParentWnd, pContext))
	{
		return FALSE;   // will self destruct on failure normally
	}

	// it worked !
	return TRUE;
}

void CMDIChildWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// update our parent frame - in case we are now maximized or not
	GetMDIFrame()->OnUpdateFrameTitle(TRUE);
}

BOOL CMDIChildWnd::UpdateClientEdge(LPRECT /*lpRect*/)
{
	// only adjust for active MDI child window
	//CMDIFrameWnd* pFrameWnd = GetMDIFrame();
	//CMDIChildWnd* pChild = pFrameWnd->MDIGetActive();

	// Only adjust for regular MDI child windows, not tabbed windows.  Attempting to set WS_EX_CLIENTEDGE on the tabbed
	// MDI client area window is subverted by CMDIClientAreaWnd::OnStyleChanging, so we always try to reset the style and
	// always repaint, none of which is necessary since the tabbed MDI children never change from maximized to restored.
	//CMDIChildWndEx* pChildEx = (pChild == NULL) ? NULL : DYNAMIC_DOWNCAST(CMDIChildWndEx, pChild);
	//BOOL bIsTabbedMDIChild = (pChildEx == NULL) ? FALSE : pChildEx->GetMDIFrameWndEx() != NULL && pChildEx->GetMDIFrameWndEx()->AreMDITabs();
	//if ((pChild == NULL || pChild == this) && !bIsTabbedMDIChild)
	//{
	//	// need to adjust the client edge style as max/restore happens
	//	DWORD dwStyle = ::GetWindowLong(pFrameWnd->m_hWndMDIClient, GWL_EXSTYLE);
	//	DWORD dwNewStyle = dwStyle;
	//	if (pChild != NULL && !(GetExStyle() & WS_EX_CLIENTEDGE) && (GetStyle() & WS_MAXIMIZE))
	//	{
	//		dwNewStyle &= ~(WS_EX_CLIENTEDGE);
	//	}
	//	else
	//	{
	//		dwNewStyle |= WS_EX_CLIENTEDGE;
	//	}

	//	if (dwStyle != dwNewStyle)
	//	{
	//		// SetWindowPos will not move invalid bits
	//		::RedrawWindow(pFrameWnd->m_hWndMDIClient, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN);

	//		// remove/add WS_EX_CLIENTEDGE to MDI client area
	//		::SetWindowLong(pFrameWnd->m_hWndMDIClient, GWL_EXSTYLE, dwNewStyle);
	//		::SetWindowPos(pFrameWnd->m_hWndMDIClient, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS);

	//		// return new client area
	//		if (lpRect != NULL)
	//		{
	//			::GetClientRect(pFrameWnd->m_hWndMDIClient, lpRect);
	//		}

	//		return TRUE;
	//	}
	//}

	return FALSE;
}

void CMDIChildWnd::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
	if (!(lpWndPos->flags & SWP_NOSIZE))
	{
		CRect rectClient;
		if (UpdateClientEdge(rectClient) && (GetStyle() & WS_MAXIMIZE))
		{
			// adjust maximized window size and position based on new
			//  size/position of the MDI client area.
			::AdjustWindowRectEx(rectClient, GetStyle(), FALSE, GetExStyle());
			lpWndPos->x = rectClient.left;
			lpWndPos->y = rectClient.top;
			lpWndPos->cx = rectClient.Width();
			lpWndPos->cy = rectClient.Height();
		}
	}

	CFrameWnd::OnWindowPosChanging(lpWndPos);
}

void CMDIChildWnd::OnDestroy()
{
	UpdateClientEdge();

	CFrameWnd::OnDestroy();
}

BOOL CMDIChildWnd::OnNcActivate(BOOL bActive)
{
	// bypass CFrameWnd::OnNcActivate()
	return CWnd::OnNcActivate(bActive);
}

int CMDIChildWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int nResult = CFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	if (nResult == MA_NOACTIVATE || nResult == MA_NOACTIVATEANDEAT)
		return nResult;   // frame does not want to activate

	// activate this window if necessary
	CMDIFrameWnd* pFrameWnd = GetMDIFrame();
	ENSURE_VALID(pFrameWnd);
	CMDIChildWnd* pActive = pFrameWnd->MDIGetActive();
	if (pActive != this)
		MDIActivate();

	return nResult;
}

BOOL CMDIChildWnd::OnToolTipText(UINT msg, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	UNUSED(pNMHDR);

	// check to see if the message is going directly to this window or not
	const MSG* pMsg = GetCurrentMessage();
	if (pMsg->hwnd != m_hWnd)
	{
		// let top level frame handle this for us
		return FALSE;
	}

	// otherwise, handle it ourselves
	return CFrameWnd::OnToolTipText(msg, pNMHDR, pResult);
}

void CMDIChildWnd::ActivateFrame(int nCmdShow)
{
	BOOL bVisibleThen = (GetStyle() & WS_VISIBLE) != 0;
	CMDIFrameWnd* pFrameWnd = GetMDIFrame();
	ASSERT_VALID(pFrameWnd);

	// determine default show command
	if (nCmdShow == -1)
	{
		// get maximized state of frame window (previously active child)
		BOOL bMaximized;
		pFrameWnd->MDIGetActive(&bMaximized);

		// convert show command based on current style
		DWORD dwStyle = GetStyle();
		if (bMaximized || (dwStyle & WS_MAXIMIZE))
			nCmdShow = SW_SHOWMAXIMIZED;
		else if (dwStyle & WS_MINIMIZE)
			nCmdShow = SW_SHOWMINIMIZED;
	}

	// finally, show the window
	CFrameWnd::ActivateFrame(nCmdShow);

	// update the Window menu to reflect new child window
	CMDIFrameWnd* pFrame = GetMDIFrame();
	::SendMessage(pFrame->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);

	// Note: Update the m_bPseudoInactive flag.  This is used to handle the
	//  last MDI child getting hidden.  Windows provides no way to deactivate
	//  an MDI child window.

	BOOL bVisibleNow = (GetStyle() & WS_VISIBLE) != 0;
	if (bVisibleNow == bVisibleThen)
		return;

	if (!bVisibleNow)
	{
		// get current active window according to Windows MDI
		HWND hWnd = (HWND)::SendMessage(pFrameWnd->m_hWndMDIClient,
			WM_MDIGETACTIVE, 0, 0);
		if (hWnd != m_hWnd)
		{
			// not active any more -- window must have been deactivated
			ASSERT(!m_bPseudoInactive);
			return;
		}

		// check next window
		ASSERT(hWnd != NULL);
		pFrameWnd->MDINext();

		// see if it has been deactivated now...
		hWnd = (HWND)::SendMessage(pFrameWnd->m_hWndMDIClient,
			WM_MDIGETACTIVE, 0, 0);
		if (hWnd == m_hWnd)
		{
			// still active -- fake deactivate it
			ASSERT(hWnd != NULL);
			::SendMessage(pFrameWnd->m_hWndMDIClient, WM_MDIACTIVATE, (WPARAM)m_hWnd, NULL);
			m_bPseudoInactive = TRUE;   // so MDIGetActive returns NULL
		}
	}
	else if (m_bPseudoInactive)
	{
		// if state transitioned from not visible to visible, but
		//  was pseudo deactivated -- send activate notify now
		::SendMessage(pFrameWnd->m_hWndMDIClient, WM_MDIACTIVATE, NULL, (LPARAM)m_hWnd);
		ASSERT(!m_bPseudoInactive); // should get set in OnMDIActivate!
	}
}

void CMDIChildWnd::SetHandles(HMENU hMenu, HACCEL hAccel)
{
	m_hMenuShared = hMenu;
	m_hAccelTable = hAccel;
}

/////////////////////////////////////////////////////////////////////////////
// CMDIChildWnd Diagnostics

#ifdef _DEBUG
void CMDIChildWnd::AssertValid() const
{
	CFrameWnd::AssertValid();
	ASSERT(m_hMenuShared == NULL || ::IsMenu(m_hMenuShared));
}

void CMDIChildWnd::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);

	dc << "m_hMenuShared = " << (void*)m_hMenuShared;
	dc << "\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Smarts for the "Window" menu

HMENU CMDIFrameWnd::GetWindowMenuPopup(HMENU hMenuBar)
	// find which popup is the "Window" menu
{
	if (hMenuBar == NULL)
		return NULL;

	ASSERT(::IsMenu(hMenuBar));

	int iItem = ::GetMenuItemCount(hMenuBar);
	while (iItem--)
	{
		HMENU hMenuPop = ::GetSubMenu(hMenuBar, iItem);
		if (hMenuPop != NULL)
		{
			int iItemMax = ::GetMenuItemCount(hMenuPop);
			for (int iItemPop = 0; iItemPop < iItemMax; iItemPop++)
			{
				UINT nID = GetMenuItemID(hMenuPop, iItemPop);
				if (nID >= AFX_IDM_WINDOW_FIRST && nID <= AFX_IDM_WINDOW_LAST)
					return hMenuPop;
			}
		}
	}

	// no default menu found
	TRACE(traceAppMsg, 0, "Warning: GetWindowMenuPopup failed!\n");
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Smarts for updating the window menu based on the current child

void CMDIFrameWnd::OnUpdateFrameMenu(HMENU hMenuAlt)
{
	CMDIChildWnd* pActiveWnd = MDIGetActive();
	if (pActiveWnd != NULL)
	{
		// let child update the menu bar
		pActiveWnd->OnUpdateFrameMenu(TRUE, pActiveWnd, hMenuAlt);
	}
	else
	{
		// no child active, so have to update it ourselves
		//  (we can't send it to a child window, since pActiveWnd is NULL)
		if (hMenuAlt == NULL)
			hMenuAlt = m_hMenuDefault;  // use default
		::SendMessage(m_hWndMDIClient, WM_MDISETMENU, (WPARAM)hMenuAlt, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// MDI Child Extensions

// walk up two parents for MDIFrame that owns MDIChild (skip MDIClient)
CMDIFrameWnd* CMDIChildWnd::GetMDIFrame()
{
	ASSERT_KINDOF(CMDIChildWnd, this);
	ASSERT(m_hWnd != NULL);
	HWND hWndMDIClient = ::GetParent(m_hWnd);
	ASSERT(hWndMDIClient != NULL);

	CMDIFrameWnd* pMDIFrame;
	pMDIFrame = (CMDIFrameWnd*)CWnd::FromHandle(::GetParent(hWndMDIClient));
	ASSERT(pMDIFrame != NULL);
	ASSERT_KINDOF(CMDIFrameWnd, pMDIFrame);
	ASSERT(pMDIFrame->m_hWndMDIClient == hWndMDIClient);
	ASSERT_VALID(pMDIFrame);
	return pMDIFrame;
}

CWnd* CMDIChildWnd::GetMessageBar()
{
	// status bar/message bar owned by parent MDI frame
	return GetMDIFrame()->GetMessageBar();
}

void CMDIChildWnd::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];
		if (pDocument == NULL)
			Checked::tcsncpy_s(szText, _countof(szText), m_strTitle, _TRUNCATE);
		else
			Checked::tcsncpy_s(szText, _countof(szText), pDocument->GetTitle(), _TRUNCATE);
		if (m_nWindow > 0)
		{
			TCHAR szWinNumber[16+1];
			_stprintf_s(szWinNumber, _countof(szWinNumber), _T(":%d"), m_nWindow);
			
			if( lstrlen(szText) + lstrlen(szWinNumber) < _countof(szText) )
			{
				Checked::tcscat_s( szText, _countof(szText), szWinNumber ); 
			}
		}

		// set title if changed, but don't remove completely
		AfxSetWindowText(m_hWnd, szText);
	}
}

void CMDIChildWnd::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd*)
{
	m_bPseudoInactive = FALSE;  // must be happening for real

	// make sure MDI client window has correct client edge
	UpdateClientEdge();

	// send deactivate notification to active view
	CView* pActiveView = GetActiveView();
	if (!bActivate && pActiveView != NULL)
		pActiveView->OnActivateView(FALSE, pActiveView, pActiveView);

	// allow hook to short circuit normal activation
	BOOL bHooked = FALSE;
#ifndef _AFX_NO_OLE_SUPPORT
	if (m_pNotifyHook != NULL && m_pNotifyHook->OnDocActivate(bActivate))
		bHooked = TRUE;
#endif

	// update titles (don't AddToTitle if deactivate last)
	if (!bHooked)
		OnUpdateFrameTitle(bActivate || (pActivateWnd != NULL));

	// re-activate the appropriate view
	if (bActivate)
	{
		if (pActiveView != NULL && GetMDIFrame() == GetActiveWindow())
			pActiveView->OnActivateView(TRUE, pActiveView, pActiveView);
	}

	// update menus
	if (!bHooked)
	{
		OnUpdateFrameMenu(bActivate, pActivateWnd, NULL);
		GetMDIFrame()->DrawMenuBar();
	}
}

void CMDIChildWnd::OnUpdateFrameMenu(BOOL bActivate, CWnd* pActivateWnd,
	HMENU hMenuAlt)
{
	CMDIFrameWnd* pFrame = GetMDIFrame();

	if (hMenuAlt == NULL && bActivate)
	{
		// attempt to get default menu from document
		CDocument* pDoc = GetActiveDocument();
		if (pDoc != NULL)
			hMenuAlt = pDoc->GetDefaultMenu();
	}

	// use default menu stored in frame if none from document
	if (hMenuAlt == NULL)
		hMenuAlt = m_hMenuShared;

	if (hMenuAlt != NULL && bActivate)
	{
		ASSERT(pActivateWnd == this);

		// activating child, set parent menu
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)hMenuAlt, (LPARAM)pFrame->GetWindowMenuPopup(hMenuAlt));
	}
	else if (hMenuAlt != NULL && !bActivate && pActivateWnd == NULL)
	{
		// destroying last child
		HMENU hMenuLast = NULL;
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)pFrame->m_hMenuDefault, (LPARAM)hMenuLast);
	}
	else
	{
		// refresh MDI Window menu (even if non-shared menu)
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
	}
}

BOOL CMDIChildWnd::OnNcCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!CFrameWnd::OnNcCreate(lpCreateStruct))
		return FALSE;

	// handle extended styles under Win4
	// call PreCreateWindow again just to get dwExStyle
	VERIFY(PreCreateWindow(*lpCreateStruct));
	SetWindowLong(m_hWnd, GWL_EXSTYLE, lpCreateStruct->dwExStyle);

	return TRUE;
}

int CMDIChildWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// call base class with lParam context (not MDI one)
	MDICREATESTRUCT* lpmcs;
	lpmcs = (MDICREATESTRUCT*)lpCreateStruct->lpCreateParams;
	CCreateContext* pContext = (CCreateContext*)lpmcs->lParam;

	return OnCreateHelper(lpCreateStruct, pContext);
}

/////////////////////////////////////////////////////////////////////////////
// Special UI processing depending on current active child

void CMDIFrameWnd::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!

#ifndef _AFX_NO_OLE_SUPPORT
	// allow hook to set the title (used for OLE support)
	if (m_pNotifyHook != NULL && m_pNotifyHook->OnUpdateFrameTitle())
		return;
#endif

	CMDIChildWnd* pActiveChild = NULL;
	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle &&
	  (pActiveChild = MDIGetActive()) != NULL &&
	  (pActiveChild->GetStyle() & WS_MAXIMIZE) == 0 &&
	  (pDocument != NULL ||
	   (pDocument = pActiveChild->GetActiveDocument()) != NULL))
		UpdateFrameTitleForDocument(pDocument->GetTitle());
	else
	{
		LPCTSTR lpstrTitle = NULL;
		CString strTitle;

		if (pActiveChild != NULL &&
			(pActiveChild->GetStyle() & WS_MAXIMIZE) == 0)
		{
			strTitle = pActiveChild->GetTitle();
			if (!strTitle.IsEmpty())
				lpstrTitle = strTitle;
		}
		UpdateFrameTitleForDocument(lpstrTitle);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Standard MDI Commands

// Two function for all standard MDI "Window" commands
void CMDIFrameWnd::OnUpdateMDIWindowCmd(CCmdUI* pCmdUI)
{
	ASSERT(m_hWndMDIClient != NULL);
	pCmdUI->Enable(MDIGetActive() != NULL);
}

BOOL CMDIFrameWnd::OnMDIWindowCmd(UINT nID)
{
	ASSERT(m_hWndMDIClient != NULL);

	UINT msg;
	UINT wParam = 0;
	switch (nID)
	{
	default:
		return FALSE;       // not for us
	case ID_WINDOW_ARRANGE:
		msg = WM_MDIICONARRANGE;
		break;
	case ID_WINDOW_CASCADE:
		msg = WM_MDICASCADE;
		break;
	case ID_WINDOW_TILE_HORZ:
		wParam = MDITILE_HORIZONTAL;
		// fall through
	case ID_WINDOW_TILE_VERT:
		ASSERT(MDITILE_VERTICAL == 0);
		msg = WM_MDITILE;
		break;
	}

	::SendMessage(m_hWndMDIClient, msg, wParam, 0);
	return TRUE;
}

void CMDIFrameWnd::OnWindowNew()
{
	CMDIChildWnd* pActiveChild = MDIGetActive();
	CDocument* pDocument;
	if (pActiveChild == NULL ||
	  (pDocument = pActiveChild->GetActiveDocument()) == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: No active document for WindowNew command.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	// otherwise we have a new frame !
	CDocTemplate* pTemplate = pDocument->GetDocTemplate();
	ASSERT_VALID(pTemplate);
	CFrameWnd* pFrame = pTemplate->CreateNewFrame(pDocument, pActiveChild);
	if (pFrame == NULL)
	{
		TRACE(traceAppMsg, 0, "Warning: failed to create new frame.\n");
		return;     // command failed
	}

	pTemplate->InitialUpdateFrame(pFrame, pDocument);
}

void CMDIFrameWnd::SetMenuBarVisibility(DWORD dwStyle)
{
	ENSURE_ARG(dwStyle == AFX_MBV_KEEPVISIBLE);
	ASSERT(m_dwMenuBarVisibility == AFX_MBV_KEEPVISIBLE);
}

BOOL CMDIFrameWnd::SetMenuBarState(DWORD dwState)
{
	return m_dwMenuBarState == AFX_MBS_HIDDEN ? FALSE : CFrameWnd::SetMenuBarState(dwState);
}

IMPLEMENT_DYNCREATE(CMDIFrameWnd, CFrameWnd)
IMPLEMENT_DYNCREATE(CMDIChildWnd, CFrameWnd)

////////////////////////////////////////////////////////////////////////////
