// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// USBOblivionDlg.cpp
//
// Copyright (c) Nikolay Raspopov, 2009-2021.
// This file is part of USB Oblivion (http://www.cherubicsoft.com/en/projects/usboblivion)
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

CUSBOblivionDlg::CUSBOblivionDlg(CWnd* pParent /*=NULL*/) noexcept
	: CDialog			( CUSBOblivionDlg::IDD, pParent )
	, m_hIcon			( AfxGetApp()->LoadIcon( IDR_MAINFRAME ) )
	, m_bEnable			( FALSE )
	, m_bAuto			( FALSE )
	, m_bSave			( TRUE )
	, m_bRestorePoint	( TRUE )
	, m_bElevation		( FALSE )
	, m_bSilent			( FALSE )
	, m_bReboot			( TRUE )
	, m_bCloseExplorer	( TRUE )
	, m_nSelected		( -1 )
	, m_InitialRect		( 0, 0, 0, 0 )
	, m_nDrives			( GetLogicalDrives() )
	, m_bRunning		( false )
	, m_pReportList		( new CLogList )
{
}

CUSBOblivionDlg::~CUSBOblivionDlg()
{
	locker_holder oLock( &m_pSection );

	while ( ! m_pReportList->IsEmpty() )
	{
		delete m_pReportList->RemoveHead();
	}
}

void CUSBOblivionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control( pDX, IDC_REPORT, m_pReport );
	DDX_Check( pDX, IDC_ENABLE, m_bEnable );
	DDX_Check( pDX, IDC_SAVE, m_bSave );
	DDX_Check( pDX, IDC_REBOOT, m_bReboot );
	DDX_Check( pDX, IDC_EXPLORER, m_bCloseExplorer );
}

void CUSBOblivionDlg::CopyToClipboard(const CString& sData)
{
	if ( OpenClipboard() )
	{
		if ( EmptyClipboard() )
		{
			size_t nLen = (size_t)( sData.GetLength() + 1 ) * sizeof( TCHAR );
			if ( HGLOBAL hGlob = GlobalAlloc( GMEM_FIXED, nLen ) )
			{
				CopyMemory( (char*)hGlob, (char*)(LPCTSTR)sData, nLen );
				if ( SetClipboardData( CF_UNICODETEXT, hGlob ) == nullptr )
				{
					// Ошибка
					GlobalFree( hGlob );
				}
			}
		}
		CloseClipboard();
	}
}

BEGIN_MESSAGE_MAP(CUSBOblivionDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_REPORT, &CUSBOblivionDlg::OnNMRClickReport)
	ON_COMMAND(ID_COPY, &CUSBOblivionDlg::OnCopy)
	ON_COMMAND(ID_COPY_ALL, &CUSBOblivionDlg::OnCopyAll)
	ON_WM_HELPINFO()
	ON_WM_DEVICECHANGE()
	ON_WM_TIMER()
	ON_BN_CLICKED( IDC_ENABLE, &CUSBOblivionDlg::OnBnClickedEnable )
END_MESSAGE_MAP()

BOOL CUSBOblivionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ShowWindow( ( m_bAuto && m_bSilent ) ? SW_HIDE : SW_SHOWNORMAL );
	CenterWindow();

	m_sDeleteKeyString = LoadString( IDS_DELETE_KEY );
	m_sDeleteValueString = LoadString( IDS_DELETE_VALUE );

	GetWindowRect( m_InitialRect );

	m_CtrlsResize.SetParentWnd( this );
	m_CtrlsResize.AddControl( IDC_REPORT, BIND_LEFT | BIND_RIGHT | BIND_TOP | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDOK, BIND_RIGHT | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDCANCEL, BIND_RIGHT | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDC_ENABLE, BIND_LEFT | BIND_BOTTOM);
	m_CtrlsResize.AddControl( IDC_SAVE, BIND_LEFT | BIND_BOTTOM);
	m_CtrlsResize.AddControl( IDC_REBOOT, BIND_LEFT | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDC_EXPLORER, BIND_LEFT | BIND_BOTTOM );
	m_CtrlsResize.FixControls();

	SetIcon( m_hIcon, TRUE );
	SetIcon( m_hIcon, FALSE );

	CString sTitle = LoadString( AFX_IDS_APP_TITLE );
#ifdef WIN64
	sTitle += _T(" 64-bit");
#else
	sTitle += _T(" 32-bit");
#endif
	SetWindowText( sTitle );

	CRect rc;
	m_pReport.GetClientRect( &rc );
	m_pReport.InsertColumn( 0, _T(""), LVCFMT_LEFT, rc.Width() - GetSystemMetrics( SM_CXVSCROLL ) - 4 );
	m_pReport.SetExtendedStyle( m_pReport.GetExtendedStyle() | LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP );

	static const WORD nIcons[] =
	{
		IDI_INF, IDI_WARN, IDI_ERR, IDI_SEARCH, IDI_DONE,
		IDR_MAINFRAME, IDI_REGEDIT, IDI_LOCK, IDI_EJECT
	};
	m_oImages.Create( 16, 16, ILC_COLOR32 | ILC_MASK, 0, _countof( nIcons ) ) ||
	m_oImages.Create( 16, 16, ILC_COLOR24 | ILC_MASK, 0, _countof( nIcons ) ) ||
	m_oImages.Create( 16, 16, ILC_COLOR16 | ILC_MASK, 0, _countof( nIcons ) );
	for ( int i = 0 ; i < _countof( nIcons ); ++i )
	{
		m_oImages.Add( (HICON)LoadImage( AfxGetResourceHandle(),
			MAKEINTRESOURCE( nIcons[ i ] ), IMAGE_ICON, 16, 16, LR_SHARED ) );
	}
	m_pReport.SetImageList( &m_oImages, LVSIL_SMALL );

	theApp.m_Loc.Translate( GetSafeHwnd(), CUSBOblivionDlg::IDD );

	Log( IDS_WARNING, Warning );

	UpdateData( FALSE );

	GetDlgItem( IDOK )->SendMessage( BCM_SETSHIELD, 0, ! IsProcessElevated() );
	SetDlgItemText( IDOK, LoadString( m_bEnable ? IDS_CLEAN : IDS_SIMULATE ) );

	if ( m_bAuto || m_bElevation )
		PostMessage( WM_COMMAND, IDOK );
	else
		Log( IDS_START );

	SetTimer( 1, 250, nullptr );

	return TRUE;
}

void CUSBOblivionDlg::OnDestroy()
{
	KillTimer( 1 );

	CDialog::OnDestroy();
}

void CUSBOblivionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize( nType, cx, cy );

	if ( IsWindow( m_pReport.GetSafeHwnd() ) )
	{
		m_CtrlsResize.OnSize();

		CRect rc;
		m_pReport.GetClientRect( &rc );
		m_pReport.SetColumnWidth( 0, rc.Width() - 4 );
	}
}

void CUSBOblivionDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_InitialRect.Width();
	lpMMI->ptMinTrackSize.y = m_InitialRect.Height();

	CDialog::OnGetMinMaxInfo( lpMMI );
}

void CUSBOblivionDlg::OnPaint()
{
	if ( IsIconic() )
	{
		CPaintDC dc( this );
		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect( &rect );
		int x = ( rect.Width() - cxIcon + 1 ) / 2;
		int y = ( rect.Height() - cyIcon + 1 ) / 2;
		dc.DrawIcon( x, y, m_hIcon );
	}
	else
		CDialog::OnPaint();
}

HCURSOR CUSBOblivionDlg::OnQueryDragIcon()
{
	return static_cast< HCURSOR >( m_hIcon );
}

void CUSBOblivionDlg::OnOK()
{
	CWaitCursor wc;

	UpdateData();

	m_pReport.DeleteAllItems();

	GetDlgItem( IDC_ENABLE )->EnableWindow( FALSE );
	GetDlgItem( IDC_SAVE )->EnableWindow( FALSE );
	GetDlgItem( IDC_REBOOT )->EnableWindow( FALSE );
	GetDlgItem( IDC_EXPLORER )->EnableWindow( FALSE );
	GetDlgItem( IDOK )->EnableWindow( FALSE );
	GetDlgItem( IDCANCEL )->EnableWindow( FALSE );

	// Нужен ли запуск под админом?
	if ( IsRunAsAdmin() || m_bElevation )
	{
		m_bRunning = true;
		startRunThread();
		return;
	}
	else if ( ! m_bElevation ) // исключение бесконечного перезапуска
	{
		Log( IDS_AS_ADMIN, Lock );

		CString sParams = _T("-elevation");
		if ( m_bAuto )
			sParams += _T( " -auto" );
		if ( m_bEnable )
			sParams += _T( " -enable" );
		if ( ! m_bSave )
			sParams += _T( " -nosave" );
		if ( ! m_bRestorePoint )
			sParams += _T( " -norestorepoint" );
		if ( ! m_bCloseExplorer )
			sParams += _T( " -noexplorer" );
		if ( ! m_bReboot )
			sParams += _T( " -norestart" );
		if ( m_bSilent )
			sParams += _T( " -silent" );
		sParams.AppendFormat( _T(" -lang:%x"), (int)theApp.m_Loc.GetLang() );

		CString sPath;
		GetModuleFileName( nullptr, sPath.GetBuffer( MAX_PATH + 1 ), MAX_PATH );
		sPath.ReleaseBuffer();

		// Запустить себя как админа
		SHELLEXECUTEINFO sei = { sizeof( SHELLEXECUTEINFO ) };
		sei.lpVerb = _T("runas");
		sei.lpFile = sPath;
		sei.lpParameters = sParams;
		sei.hwnd = GetSafeHwnd();
		sei.nShow = ( m_bAuto && m_bSilent ) ? SW_HIDE : SW_SHOWNORMAL;

		if ( ShellExecuteEx( &sei ) )
		{
			// Успех самозапуска
			CDialog::OnOK();
			return;
		}
	}

	GetDlgItem( IDC_ENABLE )->EnableWindow( TRUE );
	GetDlgItem( IDC_SAVE )->EnableWindow( TRUE );
	GetDlgItem( IDC_REBOOT )->EnableWindow( TRUE );
	GetDlgItem( IDC_EXPLORER )->EnableWindow( TRUE );
	GetDlgItem( IDOK )->EnableWindow( TRUE );
	GetDlgItem( IDCANCEL )->EnableWindow( TRUE );

	Log( IDS_RUN_DONE, Done );

	if ( m_bAuto )
		CDialog::OnOK();
}

void CUSBOblivionDlg::OnCancel()
{
	if ( m_bRunning )
		MessageBeep( MB_ICONHAND );
	else
		CDialog::OnCancel();
}

void CUSBOblivionDlg::RunThread()
{
	VERIFY( SUCCEEDED( CoInitializeEx( 0, COINIT_MULTITHREADED ) ) );

	// Подготовка к запуску
	if ( PrepareBackup() )
	{
		// Запуск #1: с привилегиями
		if ( ! RunAsSystem() )
			// Запуск #2: без привилегий
			Run();
	}

	// Подчистка
	FinishBackup();

	Reboot();

	if ( !m_bReboot && m_bCloseExplorer )
	{
		// Если нет перезагрузки, но Проводник был закрыт, то перезапуск Проводника
		StartExplorer();
	}

	Log( IDS_RUN_DONE, Done );

	CoUninitialize();
}

BOOL CUSBOblivionDlg::RunAsSystem()
{
	CAccessToken oToken;
    if ( oToken.GetProcessToken( TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY ) )
    {
		// Для перечисления всех процессов
		if ( ! oToken.EnablePrivilege( SE_DEBUG_NAME ) )
			TRACE( _T("EnablePrivilege(SE_DEBUG_NAME) error: %d\n"), GetLastError() );

		// Для настройки доступа к токену системного аккаунта под Win2K/WinXP
		if ( ! oToken.EnablePrivilege( SE_TAKE_OWNERSHIP_NAME ) )
			TRACE( _T("EnablePrivilege(SE_TAKE_OWNERSHIP_NAME) error: %d\n"), GetLastError() );
		if ( ! oToken.EnablePrivilege( SE_SECURITY_NAME ) )
			TRACE( _T("EnablePrivilege(SE_SECURITY_NAME) error: %d\n"), GetLastError() );

		// Для отключения дисков
		if ( ! oToken.EnablePrivilege( SE_UNDOCK_NAME ) )
			TRACE( _T("EnablePrivilege(SE_UNDOCK_NAME) error: %d\n"), GetLastError() );
		if ( ! oToken.EnablePrivilege( SE_LOAD_DRIVER_NAME ) )
			TRACE( _T("EnablePrivilege(SE_LOAD_DRIVER_NAME) error: %d\n"), GetLastError() );
	}
	else
		TRACE( _T("CAccessToken::GetProcessToken error: %d\n"), GetLastError() );

	const LPCTSTR szTargets[] = { _T("lsass.exe"), _T("smss.exe"), _T("csrss.exe"), _T("services.exe"),  _T("winlogon.exe") };

	PROCESSENTRY32 pe32 = { sizeof( PROCESSENTRY32 ) };
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( hProcessSnap != INVALID_HANDLE_VALUE )
	{
		for ( int i = 0; i < _countof( szTargets ); ++i )
		{
			if ( Process32First( hProcessSnap, &pe32 ) )
			{
				do
				{
					if ( pe32.th32ProcessID == 0 )
						continue;

					LPCTSTR szProcessName = PathFindFileName( pe32.szExeFile );
					if ( CmpStrI( szProcessName, szTargets[ i ] ) )
					{
						TRACE( _T("Found process: %s (PID: %d)\n"), szProcessName, pe32.th32ProcessID );

						if ( RunAsProcess( pe32.th32ProcessID ) )
						{
							CloseHandle( hProcessSnap );
							return TRUE;
						}
					}
				}
				while ( Process32Next( hProcessSnap, &pe32 ) );
			}
			else
				TRACE( _T("Process32First error: %u\n"), GetLastError() );
		}
		CloseHandle( hProcessSnap );
	}
	else
		TRACE( _T("CreateToolhelp32Snapshot(Process) error: %u\n"), GetLastError() );

	Log( IDS_ERROR_PROCESS_ACCESS, Error );

	return FALSE;
}

BOOL CUSBOblivionDlg::RunAsProcess(DWORD dwProcessID)
{
	BOOL bResult = FALSE;

	// Открытие найденного системного процесса чтобы "украсть" его токен
	if ( HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, dwProcessID ) )
	{
		// Нужен токен с имперсонацией
		if ( HANDLE hToken = OpenProcessToken( hProcess, TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE ) )
		{
			// Вход под системным аккаунтом
			if ( ImpersonateLoggedOnUser( hToken ) )
			{
				bResult = TRUE;

				Log( IDS_RUN_IMPERSONATE, Lock );

				Run();

				// Возврат к себе
				RevertToSelf();
			}
			else
				TRACE( _T("ImpersonateLoggedOnUser error: %u (PID: %d)\n"), GetLastError(), dwProcessID );

			CloseHandle( hToken );
		}
		else
			TRACE( _T("OpenProcessToken error: %u (PID: %d)\n"), GetLastError(), dwProcessID );

		CloseHandle( hProcess );
	}

	return bResult;
}

void CUSBOblivionDlg::Log(UINT nID, UINT nType)
{
	Log( LoadString( nID ), nType );
}

void CUSBOblivionDlg::Log(const CString& sText, UINT nType)
{
	TRACE( "[LOG] %s\r\n", (LPCSTR)CT2A( (LPCTSTR)sText ) );

	locker_holder oLock( &m_pSection );

	m_pReportList->AddTail( new CLogItem( sText, nType ) );

	if ( ! m_sLog.IsEmpty() )
	{
		TRY
		{
			CFile file( m_sLog, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone );

			file.SeekToEnd();

			if ( file.GetLength() == 0 )
				file.Write( _T("\xfeff"), 2 );

			file.Write( sText, (UINT)( sText.GetLength() * sizeof( TCHAR ) ) );
			file.Write( _T("\r\n"), (UINT)( 2 * sizeof( TCHAR ) ) );
		}
		CATCH_ALL(e)
		{
			DELETE_EXCEPTION(e);
		}
		END_CATCH_ALL
	}
}

bool CUSBOblivionDlg::EjectDrive(TCHAR DriveLetter)
{
	CString msg;

	if ( DriveLetter < _T('A') || DriveLetter > _T('Z') )
		return false;

	TCHAR szRootPath[] = _T("X:\\");				// "X:\"  -> for GetDriveType
	szRootPath[0] = DriveLetter;
	TCHAR szDevicePath[] = _T("X:");				// "X:"   -> for QueryDosDevice
	szDevicePath[0] = DriveLetter;
	TCHAR szVolumeAccessPath[] = _T("\\\\.\\X:");	// "\\.\X:"  -> to open the volume
	szVolumeAccessPath[4] = DriveLetter;

	// open the storage volume
	HANDLE hVolume = CreateFile( szVolumeAccessPath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr );
	if ( hVolume == INVALID_HANDLE_VALUE )
		return false;

	// get the volume's device number
	DWORD DeviceNumber = (DWORD)-1;
	STORAGE_DEVICE_NUMBER sdn = {};
	DWORD dwBytesReturned = 0;
	if ( DeviceIoControl( hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER, nullptr, 0, &sdn, sizeof( sdn ), &dwBytesReturned, nullptr ) )
	{
		DeviceNumber = sdn.DeviceNumber;
	}
	CloseHandle( hVolume );

	if ( DeviceNumber == (DWORD)-1 )
		return false;

	// get the drive type which is required to match the device numbers correctly
	UINT DriveType = GetDriveType( szRootPath );

	// get the dos device name (like \device\floppy0) to decide if it's a floppy or not - who knows a better way?
	TCHAR szDosDeviceName[ MAX_PATH ] = {};
	if ( ! QueryDosDevice( szDevicePath, szDosDeviceName, MAX_PATH ) )
		return false;

	// get the device instance handle of the storage volume by means of a SetupDi enum and matching the device number
	DEVINST DevInst = GetDrivesDevInstByDeviceNumber( DeviceNumber, DriveType, szDosDeviceName );
	if ( ! DevInst )
		return false;

	// get drives's parent, e.g. the USB bridge, the SATA port, an IDE channel with two drives!
	DEVINST DevParent = 0;
	if ( CM_Get_Parent( &DevParent, DevInst, 0 ) != CR_SUCCESS )
		return false;

	msg.Format( LoadString( IDS_RUN_EJECT ), DriveLetter );
	Log( msg, Eject );

	if ( ! m_bEnable )
		// Simulation only
		return true;

	for ( int tries = 1; tries <= 3; ++tries )
	{
		// sometimes we need some tries...
		PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;
		TCHAR szVetoName[ MAX_PATH ] = {};

		// CM_Query_And_Remove_SubTree doesn't work for restricted users
		// CM_Query_And_Remove_SubTreeW(DevParent, &VetoType, VetoNameW, MAX_PATH, CM_REMOVE_NO_RESTART); // CM_Query_And_Remove_SubTreeA is not implemented under W2K!
		// CM_Query_And_Remove_SubTreeW(DevParent, NULL, NULL, 0, CM_REMOVE_NO_RESTART);  // with messagebox (W2K, Vista) or balloon (XP)

		// CM_Request_Device_EjectW(DevParent, NULL, NULL, 0, 0); // with messagebox (W2K, Vista) or balloon (XP)
		if ( CM_Request_Device_Eject( DevParent, &VetoType, szVetoName, MAX_PATH, 0 ) == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown )
		{
			msg.Format( LoadString( IDS_DISK_UNMOUNT ), DriveLetter );
			Log( msg );
			return true;
		}

		Sleep( 500 ); // required to give the next tries a chance!
	}

	msg.Format( LoadString( IDS_ERROR_EJECT ), DriveLetter );
	Log( msg, Error );

	return false;
}

void CUSBOblivionDlg::Run()
{
	CString msg;

	Log( m_bEnable ? IDS_MODE_WORK : IDS_MODE_SIM );

	VERIFY( InitializeCOMSecurity() );

	StopServices();

	EjectDrives();

	CloseExplorer();

	// Creation of System Restore Point - begin
	RESTOREPOINTINFOW RestorePtInfo = { BEGIN_SYSTEM_CHANGE, DEVICE_DRIVER_INSTALL };
	wcscpy_s( RestorePtInfo.szDescription, AfxGetAppName() );
	STATEMGRSTATUS SMgrStatus = { ERROR_SERVICE_DISABLED };
	if ( theApp.dyn_SRSetRestorePointW && m_bRestorePoint )
	{
		Log( IDS_RESTORE_POINT );

		if ( theApp.dyn_SRSetRestorePointW( &RestorePtInfo, &SMgrStatus ) )
		{
			// OK
		}
		else if ( SMgrStatus.nStatus == ERROR_SERVICE_DISABLED )
		{
			Log( IDS_DISABLED_RESTORE_POINT, Warning );
		}
		else
		{
			msg.Format( _T( "%u" ), SMgrStatus.nStatus );
			Log( LoadString( IDS_ERROR_RESTORE_POINT ) + msg, Error );
		}
	}

	CleanFiles();

	CleanLogs();

	CleanLocalMachine();

	CleanMountedDevices();

	CleanWindowsSearch();

	CleanUsers();

	// Creation of System Restore Point - end
	RestorePtInfo.dwEventType = END_SYSTEM_CHANGE;
	if ( theApp.dyn_SRSetRestorePointW && m_bRestorePoint && SMgrStatus.nStatus == ERROR_SUCCESS )
	{
		if ( theApp.dyn_SRSetRestorePointW( &RestorePtInfo, &SMgrStatus ) )
		{
			Log( IDS_OK_RESTORE_POINT, Done );
		}
		else
		{
			msg.Format( _T("%u"), SMgrStatus.nStatus );
			Log( LoadString( IDS_ERROR_RESTORE_POINT ) + msg, Error );
		}
	}
}

void CUSBOblivionDlg::DoDeleteFile(LPCTSTR szPath)
{
	CFileFind ff;
	BOOL bWorking = ff.FindFile( szPath );
	while ( bWorking )
	{
		bWorking = ff.FindNextFile();
		const CString sPath = ff.GetFilePath();
		if ( ! ff.IsDirectory() )
		{
			if ( m_bEnable )
			{
				const CString sLongPath = CString( _T("\\\\?\\") ) + sPath;
				if ( ::DeleteFile( sLongPath ) )
				{
					Log( LoadString( IDS_DELETE_FILE ) + sPath, Clean );
				}
				else
				{
					if ( ::MoveFileEx( sLongPath, nullptr, MOVEFILE_DELAY_UNTIL_REBOOT ) )
					{
						Log( LoadString( IDS_DELETE_FILE_BOOT ) + sPath, Warning );
					}
					else
					{
						Log( LoadString( IDS_DELETE_FILE_ERROR ) + sPath, Error );
					}
				}
			}
			else
			{
				Log( LoadString( IDS_DELETE_FILE ) + sPath, Clean );
			}
		}
	}
}

void CUSBOblivionDlg::DoDeleteLog(LPCTSTR szName)
{
	if ( m_bEnable )
	{
		// Using external utility - https://technet.microsoft.com/en-us/library/cc732848(v=ws.11).aspx
		TCHAR szCommand[ MAX_PATH ] = _T("wevtutil.exe cl ");
		_tcscat_s( szCommand, szName );

		STARTUPINFO si = { sizeof( STARTUPINFO ) };
		si.dwFlags = STARTF_USESHOWWINDOW;
		PROCESS_INFORMATION pi = {};
		if ( CreateProcess( nullptr, szCommand, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi ) )
		{
			// Wait until child process exits
			WaitForSingleObject( pi.hProcess, INFINITE );

			// Close process and thread handles
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}

		// Using API
		if ( HANDLE hLog = OpenEventLog( nullptr, szName ) )
		{
			DWORD dwCount = 0;
			if ( GetNumberOfEventLogRecords( hLog, &dwCount ) && dwCount > 1 )
			{
				if ( ClearEventLog( hLog, nullptr ) )
				{
					Log( LoadString( IDS_RUN_LOG ) + szName, Clean );
				}
				else
				{
					CString msg;
					msg.Format( _T("%s %s (%u)"), (LPCTSTR)LoadString( IDS_RUN_LOG_ERROR ), szName, GetLastError() );
					Log( msg, Error );
				}
			}
			CloseEventLog( hLog );
		}
		else
		{
			CString msg;
			msg.Format( _T("%s %s (%u)"), (LPCTSTR)LoadString( IDS_RUN_LOG_ERROR ), szName, GetLastError() );
			Log( msg, Error );
		}
	}
	else
	{
		Log( LoadString( IDS_RUN_LOG ) + szName, Clean );
	}
}

void CUSBOblivionDlg::OnNMRClickReport(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if ( pNMItemActivate->iItem >= 0 )
	{
		m_nSelected = pNMItemActivate->iItem;

		ClientToScreen( &pNMItemActivate->ptAction );

		CMenu oMenu;
		oMenu.LoadMenu( IDR_CONTEXT );
		theApp.m_Loc.Translate( oMenu.GetSafeHmenu(), IDR_CONTEXT );
		oMenu.GetSubMenu( 0 )->TrackPopupMenu( TPM_LEFTALIGN |TPM_RIGHTBUTTON,
			pNMItemActivate->ptAction.x,
			pNMItemActivate->ptAction.y, this );
	}

	*pResult = 0;
}

void CUSBOblivionDlg::OnCopy()
{
	if ( m_nSelected >= 0 )
	{
		CopyToClipboard( m_pReport.GetItemText( m_nSelected, 0 ) );
	}
}

void CUSBOblivionDlg::OnCopyAll()
{
	CString sData;
	int nCount = m_pReport.GetItemCount();
	for ( int i = 0; i < nCount; ++i )
	{
		sData += m_pReport.GetItemText( i, 0 ) + _T("\r\n");
	}
	CopyToClipboard( sData );
}

BOOL CUSBOblivionDlg::OnHelpInfo(HELPINFO* /*pHelpInfo*/)
{
	CWaitCursor wc;

	ShellExecute( GetSafeHwnd(), nullptr, _T("http://www.cherubicsoft.com/projects/usboblivion"), nullptr, nullptr, SW_SHOWDEFAULT );

	return TRUE;
}

BOOL CUSBOblivionDlg::OnDeviceChange(UINT /*nEventType*/, DWORD_PTR /*dwData*/)
{
	DWORD nDrives = GetLogicalDrives();
	if ( m_nDrives != nDrives )
	{
		DWORD nMask = 1;
		for ( TCHAR i = _T('A'); i <= _T('Z'); ++i, nMask <<= 1 )
		{
			if ( ( ( m_nDrives ^ nDrives ) & nMask ) != 0 )
			{
				CString msg;
				if ( ( m_nDrives & nMask ) == 0 )
					msg.Format( LoadString( IDS_DISK_MOUNT ), i );
				else
					msg.Format( LoadString( IDS_DISK_UNMOUNT ), i );
				Log( msg );
			}
		}

		m_nDrives = nDrives;
	}
	return TRUE;
}

void CUSBOblivionDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Get completed log for output and create new one
	CAutoPtr< CLogList > pCopy;
	{
		locker_holder oLock( &m_pSection );
		if ( ! m_pReportList->IsEmpty() )
		{
			pCopy.Attach( m_pReportList.Detach() );
			m_pReportList.Attach( new CLogList );
		}
	}

	// Output log on screen
	if ( pCopy && ! pCopy->IsEmpty() )
	{
		DWORD nInsert = m_pReport.GetItemCount();
		do
		{
			CLogItem* pItem = pCopy->RemoveHead();
			m_pReport.InsertItem( nInsert++, pItem->first, pItem->second );
			delete pItem;
		}
		while ( ! pCopy->IsEmpty() );

		m_pReport.EnsureVisible( nInsert - 1, FALSE );
	}

	if ( m_bRunning && ! m_threadRunThread.is_running() )
	{
		// Complete
		m_bRunning = false;

		m_threadRunThread.close();

		GetDlgItem( IDC_ENABLE )->EnableWindow( TRUE );
		GetDlgItem( IDC_SAVE )->EnableWindow( TRUE );
		GetDlgItem( IDC_REBOOT )->EnableWindow( TRUE );
		GetDlgItem( IDC_EXPLORER )->EnableWindow( TRUE );
		GetDlgItem( IDOK )->EnableWindow( TRUE );
		GetDlgItem( IDCANCEL )->EnableWindow( TRUE );

		if ( m_bAuto )
			CDialog::OnOK();
	}

	CDialog::OnTimer( nIDEvent );
}

INT_PTR CUSBOblivionDlg::DoModal()
{
	// can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != nullptr || m_hDialogTemplate != nullptr ||
		m_lpDialogTemplate != nullptr);

	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != nullptr)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != nullptr)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == nullptr)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
#ifndef _AFX_NO_OLE_SUPPORT
	CWnd* pMainWnd = nullptr;
	BOOL bEnableMainWnd = FALSE;
#endif
	if (hWndParent && hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
#ifndef _AFX_NO_OLE_SUPPORT
		pMainWnd = AfxGetMainWnd();
		if (pMainWnd && pMainWnd->IsFrameWnd() && pMainWnd->IsWindowEnabled())
		{
			//
			// We are hosted by non-MFC container
			//
			pMainWnd->EnableWindow(FALSE);
			bEnableMainWnd = TRUE;
		}
#endif
	}

	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
						CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = 0;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != nullptr)
				SetWindowPos(nullptr, 0, 0, 0, 0, SWP_HIDEWINDOW|
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION(e);
		m_nModalResult = -1;
	}
	END_CATCH_ALL

#ifndef _AFX_NO_OLE_SUPPORT
	if (bEnableMainWnd)
		pMainWnd->EnableWindow(TRUE);
#endif
	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
	if (hWndParent != nullptr && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	// unlock/free resources as necessary
	if (m_lpszTemplateName != nullptr || m_hDialogTemplate != nullptr)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != nullptr)
		FreeResource(hDialogTemplate);

	return m_nModalResult;
}

void CUSBOblivionDlg::OnBnClickedEnable()
{
	UpdateData();

	SetDlgItemText( IDOK, LoadString( m_bEnable ? IDS_CLEAN : IDS_SIMULATE ) );
}
