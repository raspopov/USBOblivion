//
// USBOblivion.cpp
//
// Copyright (c) Nikolay Raspopov, 2009-2011.
// This file is part of USB Oblivion (http://code.google.com/p/usboblivion/)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "stdafx.h"
#include "USBOblivion.h"
#include "USBOblivionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString LoadString(UINT nID)
{
	return theApp.m_Loc.LoadString( nID );
}

BEGIN_MESSAGE_MAP(CUSBOblivionApp, CWinApp)
END_MESSAGE_MAP()

CUSBOblivionApp theApp;

CUSBOblivionApp::CUSBOblivionApp()
{
}

BOOL CUSBOblivionApp::InitInstance()
{
	const INITCOMMONCONTROLSEX InitCtrls =
	{
		sizeof( INITCOMMONCONTROLSEX ), ICC_WIN95_CLASSES
	};
	InitCommonControlsEx( &InitCtrls );

	CWinApp::InitInstance();

	SetRegistryKey( _T("USBOblivion") );

	m_Loc.Load();
	m_Loc.Select();

	CUSBOblivionDlg dlg;

	BOOL bExit = FALSE;
	int nArgs = 0;
	LPWSTR* szArglist = CommandLineToArgvW( GetCommandLineW(), &nArgs );
	if ( szArglist )
	{
		for( int i = 0; i < nArgs; ++i )
		{
			if ( CmpStrI( szArglist[ i ], _T("-?") ) ||
				 CmpStrI( szArglist[ i ], _T("/?") ) )
			{
				AfxMessageBox( IDS_ABOUT );
				bExit = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-enable") )||
				CmpStrI( szArglist[ i ], _T("/enable") ) )
			{
				dlg.m_bEnable = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-auto") ) ||
				CmpStrI( szArglist[ i ], _T("/auto") ) )
			{
				dlg.m_bAuto = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-nosave") ) ||
				CmpStrI( szArglist[ i ], _T("/nosave") ) )
			{
				dlg.m_bSave = FALSE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-elevation") ) ||
				CmpStrI( szArglist[ i ], _T("/elevation") ) )
			{
				dlg.m_bElevation = TRUE;
			}
		}
	}
	LocalFree( szArglist );

	if ( ! bExit )
	{
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}

	return FALSE;
}

int CUSBOblivionApp::ExitInstance()
{
	CWinApp::ExitInstance();

	// Всегда возврат 0, для нормального запуска через другие программы
	return 0;
}
