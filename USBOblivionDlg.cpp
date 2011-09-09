//
// USBOblivionDlg.cpp
//
// Copyright (c) Nikolay Raspopov, 2009-2011.
// This file is part of USB Oblivion (www.cherubicsoft.com)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "stdafx.h"
#include "USBOblivion.h"
#include "USBOblivionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const LPCTSTR szSYS		= _T("SYSTEM");
const LPCTSTR szMD		= _T("SYSTEM\\MountedDevices");
const LPCTSTR szMP2		= _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2");
const LPCTSTR szCPC		= _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2\\CPC\\Volume");

static const CKeyDef defs[] =
{
	// "Universal Serial Bus controllers" XP, Vista
	{ mControlSet, _T("Control\\Class\\{36FC9E60-C465-11CF-8056-444553540000}"), NULL, _T("InfSection"), _T("USBSTOR_BULK"), FALSE },
	// "DVD/CD-ROM drives" XP, Vista
	//{ mControlSet, _T("Control\\Class\\{4D36E965-E325-11CE-BFC1-08002BE10318}"), NULL, _T("InfSection"), _T("cdrom_install"), FALSE },
	// "Disk drives" XP, Vista
	//{ mControlSet, _T("Control\\Class\\{4D36E967-E325-11CE-BFC1-08002BE10318}"), NULL, _T("InfSection"), _T("disk_install"), FALSE },
	// "Portable Devices" Vista
	{ mControlSet, _T("Control\\Class\\{EEC5AD98-8080-425F-922A-DABF3DE3F69A}"), NULL, _T("InfSection"), _T("Basic_Install"), FALSE },
	// XP, Vista
	{ mControlSet, _T("Control\\DeviceClasses\\{53f56307-b6bf-11d0-94f2-00a0c91efb8b}"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Control\\DeviceClasses\\{53f56308-b6bf-11d0-94f2-00a0c91efb8b}"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// XP
	{ mControlSet, _T("Control\\DeviceClasses\\{53f5630a-b6bf-11d0-94f2-00a0c91efb8b}"), _T("STORAGE#RemovableMedia"), NULL, NULL, FALSE },
	// XP, Vista
	{ mControlSet, _T("Control\\DeviceClasses\\{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"), _T("STORAGE#RemovableMedia"), NULL, NULL, FALSE },
	{ mControlSet, _T("Control\\DeviceClasses\\{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Control\\DeviceClasses\\{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// Vista
	{ mControlSet, _T("Control\\DeviceClasses\\{6ac27878-a6fa-4155-ba85-f98f491d4f33}"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Control\\DeviceClasses\\{6ac27878-a6fa-4155-ba85-f98f491d4f33}"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// Vista
	{ mControlSet, _T("Control\\DeviceClasses\\{f33fdc04-d1ac-4e8e-9a30-19bbd4b108ae}"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Control\\DeviceClasses\\{f33fdc04-d1ac-4e8e-9a30-19bbd4b108ae}"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// Windows 7
	{ mControlSet, _T("Control\\DeviceClasses\\{10497b1b-ba51-44e5-8318-a65c837b6661}"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Control\\DeviceClasses\\{10497b1b-ba51-44e5-8318-a65c837b6661}"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// XP
	{ mControlSet, _T("Enum\\STORAGE\\RemovableMedia"), NULL, NULL, NULL, FALSE },
	// Vista
	{ mControlSet, _T("Enum\\STORAGE\\Volume"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Enum\\STORAGE\\Volume"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// XP, Vista
	{ mControlSet, _T("Enum\\USBSTOR"), NULL, NULL, NULL, TRUE },
	{ mControlSet, _T("Services\\USBSTOR\\Enum"), NULL, NULL, NULL, TRUE },
	{ mControlSet, _T("Control\\usbflags"), NULL, NULL, NULL, TRUE },
	// Vista
	{ mControlSet, _T("Enum\\WpdBusEnumRoot\\UMB"), _T("USBSTOR#Disk"), NULL, NULL, FALSE },
	{ mControlSet, _T("Enum\\WpdBusEnumRoot\\UMB"), _T("USBSTOR#CdRom"), NULL, NULL, FALSE },
	// Vista
	{ mControlSet, _T("Hardware Profiles\\0000\\System\\CurrentControlSet\\Enum\\USBSTOR"), NULL, NULL, NULL, TRUE },
	{ mControlSet, _T("Hardware Profiles\\0001\\System\\CurrentControlSet\\Enum\\USBSTOR"), NULL, NULL, NULL, TRUE },
	{ mControlSet, _T("Hardware Profiles\\0002\\System\\CurrentControlSet\\Enum\\USBSTOR"), NULL, NULL, NULL, TRUE },
	// Vista
	{ mHKLM_Val, _T("SOFTWARE\\Microsoft\\WBEM\\WDM"), _T("USBSTOR"), NULL, NULL, FALSE },
	// Vista
	{ mHKLM_Val, _T("SOFTWARE\\Microsoft\\WBEM\\WDM\\DREDGE"), _T("USBSTOR"), NULL, NULL, FALSE },
	// Vista ReadyBoost
	{ mHKLM_Key, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\EMDMgmt"), NULL, NULL, NULL, FALSE },
	// Vista
	{ mHKLM_Key, _T("SOFTWARE\\Microsoft\\Windows Portable Devices\\Devices"), NULL, NULL, NULL, FALSE },
	// AutoPlay
	{ mHKCU_Key, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers\\KnownDevices"), NULL, NULL, NULL, TRUE },
	// SafelyRemove
	{ mHKCU_Key, _T("SOFTWARE\\Software\\SafelyRemove\\Drives"), NULL, NULL, NULL, TRUE },
	// MuiCache
	{ mHKCU_Key, _T("SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache"), NULL, NULL, NULL, TRUE },
	// HandlerInstances
	{ mHKCU_Key, _T("SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\SyncMgr\\HandlerInstances"), NULL, NULL, NULL, TRUE },
	// end
	{ mControlSet, NULL, NULL, NULL, NULL, FALSE }
};


CUSBOblivionDlg::CUSBOblivionDlg(CWnd* pParent /*=NULL*/)
	: CDialog		( CUSBOblivionDlg::IDD, pParent )
	, m_hIcon		( AfxGetApp()->LoadIcon( IDR_MAINFRAME ) )
	, m_bEnable		( FALSE )
	, m_bAuto		( FALSE )
	, m_bSave		( TRUE )
	, m_bElevation	( FALSE )
	, m_nSelected	( -1 )
	, m_InitialRect	( 0, 0, 0, 0 )
	, m_nDrives		( GetLogicalDrives() )
{
	(FARPROC&)m_pRegDeleteKeyExW = GetProcAddress(
		GetModuleHandle( _T("advapi32.dll") ), "RegDeleteKeyExW" );
}

void CUSBOblivionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_REPORT, m_pReport);
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);
	DDX_Check(pDX, IDC_SAVE, m_bSave);
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
				if ( SetClipboardData( CF_UNICODETEXT, hGlob ) == NULL )
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
END_MESSAGE_MAP()

BOOL CUSBOblivionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetWindowRect( m_InitialRect );

	m_CtrlsResize.SetParentWnd( this );
	m_CtrlsResize.AddControl( IDC_REPORT, BIND_LEFT | BIND_RIGHT | BIND_TOP | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDOK, BIND_RIGHT | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDCANCEL, BIND_RIGHT | BIND_BOTTOM );
	m_CtrlsResize.AddControl( IDC_ENABLE, BIND_LEFT | BIND_BOTTOM);
	m_CtrlsResize.AddControl( IDC_SAVE, BIND_LEFT | BIND_BOTTOM);
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
	m_pReport.InsertColumn( 0, _T(""), LVCFMT_LEFT, rc.Width() -
		GetSystemMetrics( SM_CXVSCROLL ) - 4 );
	m_pReport.SetExtendedStyle( m_pReport.GetExtendedStyle() |
		LVS_EX_DOUBLEBUFFER | LVS_EX_LABELTIP );

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

	Log( IDS_WARNING, Warning );

	UpdateData( FALSE );

	GetDlgItem( IDOK )->SendMessage( BCM_SETSHIELD, 0, ! IsProcessElevated() );

	if ( m_bAuto || m_bElevation )
		PostMessage( WM_COMMAND, IDOK );
	else
		Log( IDS_START );

	return TRUE;
}

void CUSBOblivionDlg::OnDestroy()
{
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
		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0 );
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect(&rect);
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
	GetDlgItem( IDOK )->EnableWindow( FALSE );
	GetDlgItem( IDCANCEL )->EnableWindow( FALSE );

	// Нужнен ли запуск под админом?
	if ( IsRunAsAdmin() || m_bElevation )
	{
		// Подготовка к запуску
		if ( PrepareBackup() )
		{
			// Запуск #1: с привелегиями
			if ( ! RunAsSystem() )
				// Запуск #2: без привелегий
				Run();
		}

		// Подчистка
		FinishBackup();
	}
	else if ( ! m_bElevation ) // исключение бесконечного перезапуска
	{
		Log( IDS_AS_ADMIN, Lock );

		CString sParams = _T("-elevation");
		if ( m_bAuto )
			sParams += _T(" -auto");
		if ( m_bEnable )
			sParams += _T(" -enable");
		if ( ! m_bSave )
			sParams += _T(" -nosave");

		CString sPath;
		GetModuleFileName( NULL, sPath.GetBuffer( MAX_PATH + 1 ), MAX_PATH );
		sPath.ReleaseBuffer();

		// Запустить себя как админа
		SHELLEXECUTEINFO sei = { sizeof( SHELLEXECUTEINFO ) };
		sei.lpVerb = _T("runas");
		sei.lpFile = sPath;
		sei.lpParameters = sParams;
		sei.hwnd = GetSafeHwnd();
		sei.nShow = SW_NORMAL;

		if ( ShellExecuteEx( &sei ) )
		{
			// Успех самозапуска
			CDialog::OnOK();
			return;
		}
	}

	GetDlgItem( IDOK )->EnableWindow( TRUE );
	GetDlgItem( IDCANCEL )->EnableWindow( TRUE );

	Log( IDS_RUN_DONE, Done );
	
	if ( m_bAuto )
		CDialog::OnOK();
}

bool CUSBOblivionDlg::PrepareBackup()
{
	if ( ! m_bSave )
		return true;

	CString sComputer;
	DWORD dwComputerLen = MAX_PATH;
	GetComputerName( sComputer.GetBuffer( MAX_PATH + 1 ), &dwComputerLen );
	sComputer.ReleaseBuffer();

	CString sLogFolder;
	SHGetFolderPath( GetSafeHwnd(), CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT,
		sLogFolder.GetBuffer( MAX_PATH + 1 ) );
	sLogFolder.ReleaseBuffer();
	sLogFolder.TrimRight( _T("\\") );

	CTime oTime = CTime::GetCurrentTime();
	CString sPath;
	sPath.Format(
#ifdef WIN64
		_T("%s\\USBOblivion-64-%s-%s.reg"),
#else
		_T("%s\\USBOblivion-32-%s-%s.reg"),
#endif
		(LPCTSTR)sLogFolder, (LPCTSTR)sComputer,
		(LPCTSTR)oTime.Format( _T("%y%m%d-%H%M%S") ) );

	if ( ! m_oFile.Open( sPath, CFile::modeCreate | CFile::modeWrite ) )
	{
		Log( LoadString( IDS_ERROR_FILE_CREATE ) + sPath, Error );
		return false;
	}

	try
	{
		Write( _T("\xfeffWindows Registry Editor Version 5.00\r\n") );
	}
	catch ( CException* e )
	{
		e->Delete();
		Log( LoadString( IDS_ERROR_FILE_WRITE ) + sPath, Error );
		return false;
	}

	Log( LoadString( IDS_FILE_REG ) + sPath, Regedit );

	return true;
}

void CUSBOblivionDlg::FinishBackup()
{
	if ( m_oFile != INVALID_HANDLE_VALUE )
	{
		m_oFile.Close();
	}
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

	PROCESSENTRY32 pe32 = { sizeof( PROCESSENTRY32 ) };
	HANDLE hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( hProcessSnap != INVALID_HANDLE_VALUE )
	{
		if ( Process32First( hProcessSnap, &pe32 ) )
		{
			do
			{
				if ( pe32.th32ProcessID == 0 )
					continue;

				LPCTSTR szProcessName = PathFindFileName( pe32.szExeFile );
				
				TRACE( _T("Found process: %s (PID: %d)\n"), szProcessName, pe32.th32ProcessID );

				if ( CmpStrI( szProcessName, _T("smss.exe") ) ||
					 CmpStrI( szProcessName, _T("csrss.exe") ) ||
					 CmpStrI( szProcessName, _T("lsass.exe") ) ||
					 CmpStrI( szProcessName, _T("services.exe") ) ||
					 CmpStrI( szProcessName, _T("winlogon.exe") ) )
				{
					if ( RunAsProcess( pe32.th32ProcessID ) )
						return TRUE;
				}
			}
			while ( Process32Next( hProcessSnap, &pe32 ) );
		}
		else
			TRACE( _T("Process32First error: %u\n"), GetLastError() );

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
		if ( HANDLE hToken = OpenProcessToken( hProcess,
			TOKEN_QUERY | TOKEN_IMPERSONATE | TOKEN_DUPLICATE ) )
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

void CUSBOblivionDlg::Log(LPCTSTR szText, UINT nType)
{
	TRACE( _T("[LOG] %hs\r\n"), (LPCSTR)CT2A( szText ) );

	m_pReport.EnsureVisible( m_pReport.InsertItem(
		m_pReport.GetItemCount(), szText, nType ), FALSE );

	UpdateWindow();

	Sleep( 0 );
}

void CUSBOblivionDlg::Write(LPCTSTR szText)
{
	m_oFile.Write( szText, (UINT)( _tcslen( szText ) * sizeof( TCHAR ) ) );
}

LPCTSTR CUSBOblivionDlg::GetKeyName(HKEY hRoot) const
{
	switch ( (DWORD_PTR)hRoot )
	{
	case HKEY_CLASSES_ROOT:
		return _T("HKEY_CLASSES_ROOT");
	case HKEY_CURRENT_USER:
		return _T("HKEY_CURRENT_USER");
	case HKEY_LOCAL_MACHINE:
		return _T("HKEY_LOCAL_MACHINE");
	case HKEY_USERS:
		return _T("HKEY_USERS");
	case HKEY_CURRENT_CONFIG:
		return _T("HKEY_CURRENT_CONFIG");
	default:
		ASSERT( FALSE );
		return NULL;
	}
}

LSTATUS CUSBOblivionDlg::RegOpenKeyFull(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired, PHKEY phkResult)
{
	LSTATUS ret = RegOpenKeyEx( hKey, lpSubKey, 0, samDesired |
		KEY_WOW64_32KEY | KEY_WOW64_64KEY, phkResult );
	if ( ret != ERROR_SUCCESS )
		ret = RegOpenKeyEx( hKey, lpSubKey, 0, samDesired, phkResult );
	if ( ret == ERROR_ACCESS_DENIED )
	{
		Log( LoadString( IDS_ERROR_ACCESS ) + GetKeyName( hKey ) + _T("\\") + lpSubKey, Error );
	}
	return ret;
}

LSTATUS CUSBOblivionDlg::RegDeleteValueFull(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue)
{
	HKEY hValueKey = NULL;
	LSTATUS ret = RegOpenKeyFull( hKey, lpSubKey, KEY_SET_VALUE, &hValueKey );
	if ( ret == ERROR_SUCCESS )
	{
		ret = RegDeleteValue( hValueKey, lpValue );
		if ( ret != ERROR_SUCCESS )
		{
			Log( LoadString( IDS_DELETE_ERROR ) + GetKeyName( hKey ) + _T("\\") + lpSubKey, Error );
		}
	}
	return ret;
}

LSTATUS CUSBOblivionDlg::RegDeleteKeyFull(HKEY hKey, const CString& sSubKey)
{
	LSTATUS ret;
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;

	// Перечисление всех подключей
	CStringList oSubKeys;
	{
		HKEY hSubKey = NULL;
		ret = RegOpenKeyFull( hKey, sSubKey, KEY_READ, &hSubKey );
		if ( ret != ERROR_SUCCESS )
			return ret;
		for ( DWORD dwIndex = 0; ; ++dwIndex )
		{
			cchName = _countof( pszName );
			ret = SHEnumKeyEx( hSubKey, dwIndex, pszName, &cchName );
			if ( ret != ERROR_SUCCESS )
				break;
			oSubKeys.AddTail( sSubKey + _T("\\") + pszName );
		}
		RegCloseKey( hSubKey );
	}

	// Удаление всех подключей
	for ( POSITION pos = oSubKeys.GetHeadPosition(); pos; )
	{
		ret = RegDeleteKeyFull( hKey, oSubKeys.GetNext( pos ) );
		if ( ret != ERROR_SUCCESS )
			return ret;
	}

	// Удаление самого ключа
	if ( m_bEnable )
	{
		if ( m_pRegDeleteKeyExW )
		{
			ret = m_pRegDeleteKeyExW( hKey, sSubKey,
				KEY_WOW64_32KEY | KEY_WOW64_64KEY, 0 );
			if ( ret != ERROR_SUCCESS )
				ret = m_pRegDeleteKeyExW( hKey, sSubKey, 0, 0 );
		}
		else
			ret = RegDeleteKeyW( hKey, sSubKey );
		if ( ret == ERROR_ACCESS_DENIED )
		{
			Log( LoadString( IDS_DELETE_ERROR ) + GetKeyName( hKey ) + _T("\\") + sSubKey, Error );
		}
	}
	else
		// Симуляция
		ret = ERROR_SUCCESS;

	return ret;
}

void CUSBOblivionDlg::ProcessKey(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringList& oKeys)
{
	CAutoVectorPtr< TCHAR > pszName( new TCHAR[ 1024 ] );
	DWORD cchName;
	DWORD dwType;
	CAutoVectorPtr< TCHAR > pszValue( new TCHAR[ 2048 ] );
	DWORD cchValue;

	CString sFullKey = sRoot + def.szKeyName;
	HKEY hKeys = NULL;
	LSTATUS ret = RegOpenKeyFull( hRoot, sFullKey, KEY_READ, &hKeys );
	if ( ret != ERROR_SUCCESS )
		return;

	DWORD dwIndex = 0, dwDeleted = 0;
	for ( ; ; ++dwIndex )
	{
		bool bAdd = false;
		pszName[ 0 ] = 0;
		cchName = 1024;
		ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
		if ( ret != ERROR_SUCCESS )
			break;
		if ( ! def.szKeySubstring || StrStrI( pszName, def.szKeySubstring ) )
		{
			if ( def.szValueName )
			{
				dwType = 0;
				pszValue[ 0 ] = 0;
				pszValue[ 1 ] = 0;
				cchValue = 4096;
				ret = SHGetValue( hKeys, pszName,
					def.szValueName, &dwType, pszValue, &cchValue );
				bAdd = ( ret == ERROR_SUCCESS &&
					( ! def.szValueSubstring ||
					StrStrI( (LPCTSTR)pszValue, def.szValueSubstring ) ) );
			}
			else
				bAdd = true;
		}
		if ( bAdd )
		{
			++ dwDeleted;
			AddUnique( oKeys, sFullKey + _T("\\") + pszName );
		}
	}

	if ( def.bDeleteEmpty && dwDeleted == dwIndex )
	{
		AddUnique( oKeys, sFullKey );
	}

	RegCloseKey( hKeys );
}

void CUSBOblivionDlg::ProcessValue(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringC2List& oValues)
{
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;
	DWORD dwType;
	BYTE pszValue[ 4096 ] = {};
	DWORD cchValue;

	CString sFullKey = sRoot + def.szKeyName;
	HKEY hKey = NULL;
	LRESULT ret = RegOpenKeyFull( hRoot, sFullKey, KEY_READ, &hKey );
	if ( ret != ERROR_SUCCESS )
		return;

	for ( DWORD dwIndex = 0; ; ++dwIndex )
	{
		pszName[ 0 ] = 0;
		cchName = _countof( pszName );
		dwType = 0;
		pszValue[ 0 ] = 0;
		pszValue[ 1 ] = 0;
		cchValue = sizeof( pszValue );
		ret = SHEnumValue( hKey, dwIndex, pszName, &cchName, &dwType,
			pszValue, &cchValue );
		if ( ret != ERROR_SUCCESS )
			break;
		if ( ! def.szKeySubstring || StrStrI( pszName, def.szKeySubstring ) )
		{
			oValues.AddTail( CStringC2( sFullKey, CString( pszName ) ) );
		}
	}
	RegCloseKey( hKey );
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
	HANDLE hVolume = CreateFile( szVolumeAccessPath, 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL );
	if ( hVolume == INVALID_HANDLE_VALUE )
		return false;

	// get the volume's device number
	DWORD DeviceNumber = (DWORD)-1;
	STORAGE_DEVICE_NUMBER sdn = {};
	DWORD dwBytesReturned = 0;
	if ( DeviceIoControl( hVolume, IOCTL_STORAGE_GET_DEVICE_NUMBER,
		NULL, 0, &sdn, sizeof( sdn ), &dwBytesReturned, NULL ) )
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
	DEVINST DevInst = GetDrivesDevInstByDeviceNumber( DeviceNumber,
		DriveType, szDosDeviceName );
	if ( ! DevInst )
		return false;

	// get drives's parent, e.g. the USB bridge, the SATA port, an IDE channel with two drives!
	DEVINST DevParent = 0;
	if ( CM_Get_Parent( &DevParent, DevInst, 0 ) != CR_SUCCESS )
		return false;

	msg.Format( IDS_RUN_EJECT, DriveLetter );
	Log( msg, Eject );

	for ( int tries = 1; tries <= 3; ++tries )
	{
		// sometimes we need some tries...
		PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown; 
		TCHAR szVetoName[ MAX_PATH ] = {};

		// CM_Query_And_Remove_SubTree doesn't work for restricted users
		// CM_Query_And_Remove_SubTreeW(DevParent, &VetoType, VetoNameW, MAX_PATH, CM_REMOVE_NO_RESTART); // CM_Query_And_Remove_SubTreeA is not implemented under W2K!
		// CM_Query_And_Remove_SubTreeW(DevParent, NULL, NULL, 0, CM_REMOVE_NO_RESTART);  // with messagebox (W2K, Vista) or balloon (XP)

		// CM_Request_Device_EjectW(DevParent, NULL, NULL, 0, 0); // with messagebox (W2K, Vista) or balloon (XP)
		if ( CM_Request_Device_Eject( DevParent, &VetoType,
			szVetoName, MAX_PATH, 0 ) == CR_SUCCESS &&
			VetoType == PNP_VetoTypeUnknown )
		{
			msg.Format( IDS_DISK_UNMOUNT, DriveLetter );
			Log( msg );
			return true;
		}

		Sleep( 500 ); // required to give the next tries a chance!
	}

	msg.Format( IDS_ERROR_EJECT, DriveLetter );
	Log( msg, Error );

	return false;
}

void CUSBOblivionDlg::Run()
{
	CString msg;
	LRESULT ret;
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;
	DWORD dwType;
	BYTE pszValue[ 4096 ] = {};
	DWORD cchValue;

	Log( m_bEnable ? IDS_MODE_WORK : IDS_MODE_SIM );

	// Сбор данных о дисках с попыткой извлечения
	DWORD nDrives = GetLogicalDrives();
	CString drives;
	DWORD nMask = 1;
	for ( TCHAR i = _T('A'); i <= _T('Z'); ++i, nMask <<= 1 )
	{
		if ( ( nDrives & nMask ) != 0 )
		{
			CString drive;
			drive += i;
			drive += _T(":");		

			UINT nType = GetDriveType( drive );
			if ( ( nType == DRIVE_REMOVABLE /*|| nType == DRIVE_CDROM*/ ) &&
				EjectDrive( i ) )
			{
				// Извлечено!
			}
			else
				drives += drive + _T(" ");
		}
	}

	//////////////////////////////////////////////////////////////////////
	// Чистка файлов

	Log( IDS_RUN_FILES, Search );

	CString sWindows;
	GetWindowsDirectory( sWindows.GetBuffer( MAX_PATH ), MAX_PATH );
	sWindows.ReleaseBuffer();
	DeleteFile( sWindows + _T("\\setupdi.log") );
	DeleteFile( sWindows + _T("\\setupapi.log") );
	DeleteFile( sWindows + _T("\\inf\\setupapi.dev.log") );
	DeleteFile( sWindows + _T("\\inf\\setupapi.app.log") );
	DeleteFile( sWindows + _T("\\inf\\INFCACHE.1") );
	DeleteFile( sWindows + _T("\\System32\\wbem\\Logs\\wmiprov.log") );

	//////////////////////////////////////////////////////////////////////
	// Чистка журналов

	Log( IDS_RUN_LOGS, Search );

	DeleteLog( L"HardwareEvents" );
	DeleteLog( L"Application" );
	DeleteLog( L"Security" );
	DeleteLog( L"System" );

	//////////////////////////////////////////////////////////////////////
	// Чистка реестра

	Log( IDS_RUN_REGISTRY, Search );

	Log( LoadString( IDS_RUN_DISK_FOUND ) + drives );

	// Обновление данных о дисках чтобы дважды не сообщать
	Sleep( 2000 );
	m_nDrives = GetLogicalDrives();

	//////////////////////////////////////////////////////////////////////
	// Сбор данных

	CStringList oControlSets;
	{
		HKEY hSYS = NULL;
		ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, szSYS, KEY_READ, &hSYS );
		if ( ret == ERROR_SUCCESS )
		{
			for ( DWORD dwIndex = 0; ; ++dwIndex )
			{
				cchName = _countof( pszName );
				ret = SHEnumKeyEx( hSYS, dwIndex, pszName, &cchName );
				if ( ret != ERROR_SUCCESS )
					break;
				if ( lstrlen( pszName ) == 13 &&
					 StrCmpN( pszName, _T("ControlSet"), 10 ) == 0 )
				{
					oControlSets.AddTail( pszName );
				}
			}
			RegCloseKey( hSYS );
		}
	}

	msg.Format( _T("%d"), oControlSets.GetCount() );
	Log( LoadString( IDS_RUN_CONTROLSETS_FOUND ) + msg );

	CStringList oKeys;
	CStringC2List oValues;
	for ( POSITION posControlSets = oControlSets.GetHeadPosition(); posControlSets; )
	{
		const CString sKey = CString( szSYS ) + _T("\\") +
			oControlSets.GetNext( posControlSets ) + _T("\\");

		// XP, Vista
		// Удаление всех остальных ключей "Enum\\USB\\VID_xxx\\yyy"
		// у которых значение "Service" равен "USBSTOR"
		{
			const CString sUSBKey = sKey + _T("Enum\\USB");
			HKEY hUSBKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sUSBKey, KEY_READ, &hUSBKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwUSBIndex = 0; ; ++dwUSBIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hUSBKeys, dwUSBIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					if ( CmpStrI( pszName, _T("Vid_"), 4 ) )
					{
						const CString sDevKey = sUSBKey + _T("\\") + pszName;
						HKEY hDevKeys = NULL;
						ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sDevKey, KEY_READ, &hDevKeys );
						if ( ret == ERROR_SUCCESS )
						{
							CStringList oSubKeys;
							DWORD dwDelIndex = 0, dwDeleted = 0;
							for ( ; ; ++dwDelIndex )
							{
								pszName[ 0 ] = 0;
								cchName = _countof( pszName );
								ret = SHEnumKeyEx( hDevKeys, dwDelIndex, pszName, &cchName );
								if ( ret != ERROR_SUCCESS )
									break;
								dwType = 0;
								pszValue[ 0 ] = 0;
								pszValue[ 1 ] = 0;
								cchValue = sizeof( pszValue );
								ret = SHGetValue( hDevKeys, pszName, _T("Service"),
									&dwType, pszValue, &cchValue );
								if ( ret == ERROR_SUCCESS &&
									CmpStrI( (LPCTSTR)pszValue, _T("USBSTOR") ) )
								{
									dwDeleted++;
									AddUnique( oSubKeys, sDevKey + _T("\\") + pszName );

									dwType = 0;
									pszValue[ 0 ] = 0;
									pszValue[ 1 ] = 0;
									cchValue = sizeof( pszValue );
									ret = SHGetValue( hDevKeys, pszName, _T("Driver"),
										&dwType, pszValue, &cchValue );
									if ( ret == ERROR_SUCCESS )
									{
										AddUnique( oKeys, sKey + _T("Control\\Class\\") + (LPCTSTR)pszValue );
									}
								}
							}
							RegCloseKey( hDevKeys );

							// Удалены все подключи?
							if ( dwDelIndex && dwDeleted == dwDelIndex )
							{
								// Удаление главного ключа
								AddUnique( oKeys, sDevKey );
								oSubKeys.RemoveAll();
							}

							for ( POSITION posSubKeys = oSubKeys.GetHeadPosition() ; posSubKeys; )
								AddUnique( oKeys, oSubKeys.GetNext( posSubKeys ) );
						}
					}
				}
				RegCloseKey( hUSBKeys );
			}
		}
	
		// XP, Vista
		// Удаление ссылок на драйвера (параметр "Driver") у устройств из "Enum\USBSTOR"
		{
			const CString sFullKey = sKey + _T("Enum\\USBSTOR");
			HKEY hKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sFullKey, KEY_READ, &hKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;

					const CString sSubFullKey = sFullKey + _T("\\") + pszName;
					HKEY hSubKeys = NULL;
					ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sSubFullKey, KEY_READ, &hSubKeys );
					if ( ret == ERROR_SUCCESS )
					{
						for ( DWORD dwIndex = 0; ; ++dwIndex )
						{
							pszName[ 0 ] = 0;
							cchName = _countof( pszName );
							ret = SHEnumKeyEx( hSubKeys, dwIndex, pszName, &cchName );
							if ( ret != ERROR_SUCCESS )
								break;

							dwType = 0;
							pszValue[ 0 ] = 0;
							pszValue[ 1 ] = 0;
							cchValue = sizeof( pszValue );
							ret = SHGetValue( hSubKeys, pszName, _T("Driver"),
								&dwType, pszValue, &cchValue );
							if ( ret == ERROR_SUCCESS )
							{
								AddUnique( oKeys, sKey + _T("Control\\Class\\") + (LPCTSTR)pszValue );
							}
						}
						RegCloseKey( hSubKeys );
					}
				}
				RegCloseKey( hKeys );
			}
		}

		// XP, Vista
		// Удаление связанных пар:
		// "Control\\DeviceClasses\\{a5dcbf10-6530-11d2-901f-00c04fb951ed}"
		// и "Enum\\USB"
		{
			const CString sFullKey = sKey + _T("Control\\DeviceClasses\\{a5dcbf10-6530-11d2-901f-00c04fb951ed}");
			HKEY hKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sFullKey, KEY_READ, &hKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					if ( StrStrI( pszName, _T("USB#Vid") ) )
					{
						dwType = 0;
						pszValue[ 0 ] = 0;
						pszValue[ 1 ] = 0;
						cchValue = sizeof( pszValue );
						ret = SHGetValue( hKeys, pszName, _T("DeviceInstance"),
							&dwType, pszValue, &cchValue );
						if ( ret == ERROR_SUCCESS )
						{
							const CString sEnumKey = sKey + _T("Enum\\") + (LPCTSTR)pszValue;
							dwType = 0;
							pszValue[ 0 ] = 0;
							pszValue[ 1 ] = 0;
							cchValue = sizeof( pszValue );
							ret = SHGetValue( HKEY_LOCAL_MACHINE, sEnumKey,
								_T("Service"), &dwType, pszValue, &cchValue );
							if ( ret == ERROR_SUCCESS &&
								CmpStrI( (LPCTSTR)pszValue, _T("USBSTOR") ) )
							{
								AddUnique( oKeys, sFullKey + _T("\\") + pszName );
								AddUnique( oKeys, sEnumKey );
							}
						}
					}
				}
				RegCloseKey( hKeys );
			}
		}

		// Подчистка всего остального
		for (int i = 0; defs[ i ].szKeyName; ++i )
		{
			if ( defs[ i ].nMode == mControlSet )
				ProcessKey( HKEY_LOCAL_MACHINE, sKey, defs[ i ], oKeys );
		}
	}

	// Обработка ключей
	for (int i = 0; defs[ i ].szKeyName; ++i )
	{
		if ( defs[ i ].nMode == mHKLM_Key )
			ProcessKey( HKEY_LOCAL_MACHINE, CString(), defs[ i ], oKeys );
		else if ( defs[ i ].nMode == mHKCR_Key )
			ProcessKey( HKEY_CLASSES_ROOT, CString(), defs[ i ], oKeys );
	}

	// Обработка переменных
	for (int i = 0; defs[ i ].szKeyName; ++i )
	{
		if ( defs[ i ].nMode == mHKLM_Val )
			ProcessValue( HKEY_LOCAL_MACHINE, CString(), defs[ i ], oValues );
		else if ( defs[ i ].nMode == mHKCR_Val )
			ProcessValue( HKEY_CLASSES_ROOT, CString(), defs[ i ], oValues );
	}

	msg.Format( _T("%d"), oKeys.GetCount() );
	Log( LoadString( IDS_RUN_KEYS_FOUND ) + msg );
	msg.Format( _T("%d"), oValues.GetCount() );
	Log( LoadString( IDS_RUN_VALUES_FOUND ) + msg );

	//////////////////////////////////////////////////////////////////////
	// Удаление

	if ( oKeys.GetCount() || oValues.GetCount() )
		Log( IDS_RUN_CLEAN, Clean );

	for ( POSITION posKeys = oKeys.GetHeadPosition(); posKeys; )
	{
		DeleteKey( HKEY_LOCAL_MACHINE, oKeys.GetNext( posKeys ) );
	}

	for ( POSITION posValues = oValues.GetHeadPosition(); posValues; )
	{
		const CStringC2& pValue = oValues.GetNext( posValues );
		DeleteValue( HKEY_LOCAL_MACHINE, pValue.first, pValue.second );
	}

	//////////////////////////////////////////////////////////////////////
	// Сбор данных

	Log( IDS_RUN_MOUNTS, Search );

	CStringList oMountedDevs;
	{
		HKEY hMD = NULL;
		ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, szMD, KEY_READ, &hMD );
		if ( ret == ERROR_SUCCESS )
		{
			for ( DWORD dwIndex = 0; ; ++dwIndex )
			{
				pszName[ 0 ] = 0;
				cchName = _countof( pszName );
				dwType = 0;
				pszValue[ 0 ] = 0;
				pszValue[ 1 ] = 0;
				cchValue = sizeof( pszValue );
				ret = SHEnumValue( hMD, dwIndex, pszName, &cchName, &dwType,
					pszValue, &cchValue );
				if ( ret != ERROR_SUCCESS )
					break;
				pszValue[ cchValue ] = 0;
				pszValue[ cchValue + 1 ] = 0;
				if ( StrStrI( (LPCTSTR)pszValue, _T("USBSTOR#Disk") ) ||
					 StrStrI( (LPCTSTR)pszValue, _T("USBSTOR#CdRom") ) ||
					 StrStrI( (LPCTSTR)pszValue, _T("STORAGE#RemovableMedia") ) )
				{
					oMountedDevs.AddTail( pszName );
				}
			}
			RegCloseKey( hMD );
		}
	}

	msg.Format( _T("%d"), oMountedDevs.GetCount() );
	Log( LoadString( IDS_RUN_MOUNT_FOUND ) + msg );

	//////////////////////////////////////////////////////////////////////
	// Удаление

	bool bLog = true;

	for ( POSITION posMountedDevs = oMountedDevs.GetHeadPosition(); posMountedDevs; )
	{
		POSITION posCurrent = posMountedDevs;
		CString sDev = oMountedDevs.GetNext( posMountedDevs );

		// HKEY_LOCAL_MACHINE\SYSTEM\MountedDevices
		// -> "\DosDevices\X:"
		if ( sDev.GetLength() == 14 &&
			 sDev.Left( 12 ) == _T("\\DosDevices\\") &&
			 // не трогать подключенные устройства
			( nDrives & ( 1 << ( sDev.GetAt( 12 ) - _T('A') ) ) ) == 0 )
		{
			if ( bLog )
			{
				bLog = false;
				Log( IDS_RUN_CLEAN, Clean );
			}
			DeleteValue( HKEY_LOCAL_MACHINE, szMD, sDev );
			oMountedDevs.RemoveAt( posCurrent );
		}
		// -> "\??\Volume{CLSID}"
		else if ( sDev.GetLength() == 48 &&
			sDev.Left( 11 ) == _T("\\??\\Volume{") )
		{
			if ( bLog )
			{
				bLog = false;
				Log( IDS_RUN_CLEAN, Clean );
			}
			DeleteValue( HKEY_LOCAL_MACHINE, szMD, sDev );
			oMountedDevs.RemoveAt( posCurrent );
		}
	}

	//////////////////////////////////////////////////////////////////////
	// Сбор данных

	Log( IDS_RUN_USERS, Search );

	CStringList oUsers;
	for ( DWORD dwIndex = 0; ; ++dwIndex )
	{
		cchName = _countof( pszName );
		ret = SHEnumKeyEx( HKEY_USERS, dwIndex, pszName, &cchName );
		if ( ret != ERROR_SUCCESS )
			break;
		if ( StrStrI( pszName, _T("_Classes") ) == 0 ) // исключить "_Classes"
		{
			oUsers.AddTail( pszName );
		}
	}

	msg.Format( _T("%d"), oUsers.GetCount() );
	Log( LoadString( IDS_RUN_USERS_FOUND ) + msg );

	for ( POSITION posUsers = oUsers.GetHeadPosition(); posUsers; )
	{
		CStringList oPoints, oVolumes;
		CString sKey = oUsers.GetNext( posUsers ) + _T("\\");
	
		Log( LoadString( IDS_RUN_USER ) + sKey.Left( sKey.Find( _T('\\') ) ), Search );
		
		CStringList oUKeys;
		CStringC2List oUValues;

		// Обработка ключей
		for (int i = 0; defs[ i ].szKeyName; ++i )
		{
			if ( defs[ i ].nMode == mHKCU_Key )
				ProcessKey( HKEY_USERS, sKey, defs[ i ], oUKeys );
		}

		// Обработка переменных
		for (int i = 0; defs[ i ].szKeyName; ++i )
		{
			if ( defs[ i ].nMode == mHKCU_Val )
				ProcessValue( HKEY_USERS, sKey, defs[ i ], oUValues );
		}

		{
			HKEY hMP2 = NULL;
			ret = RegOpenKeyFull( HKEY_USERS, sKey + szMP2, KEY_READ, &hMP2 );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hMP2, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					AddUnique( oPoints, sKey + szMP2 + _T("\\") + pszName );
				}
				RegCloseKey( hMP2 );
			}
		}

		msg.Format( _T("%d"), oPoints.GetCount() );
		Log( LoadString( IDS_RUN_MOUNT_FOUND ) + msg );

		{
			HKEY hCPC = NULL;
			ret = RegOpenKeyFull( HKEY_USERS, sKey + szCPC, KEY_READ, &hCPC );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hCPC, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					AddUnique( oVolumes, sKey + szCPC + _T("\\") + pszName );
				}
				RegCloseKey( hCPC );
			}
		}

		msg.Format( _T("%d"), oVolumes.GetCount() );
		Log( LoadString( IDS_RUN_EXPLORER_FOUND ) + msg );

		{
			HKEY hCPC = NULL;
			ret = RegOpenKeyFull( HKEY_USERS, sKey + szCPC, KEY_READ, &hCPC );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hCPC, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					AddUnique( oVolumes, sKey + szCPC + _T("\\") + pszName );
				}
				RegCloseKey( hCPC );
			}
		}

		//////////////////////////////////////////////////////////////////
		// Удаление

		bLog = true;

		if ( oUKeys.GetCount() || oUValues.GetCount() )
		{
			bLog = false;
			Log( IDS_RUN_CLEAN, Clean );
		}

		for ( POSITION posKeys = oUKeys.GetHeadPosition(); posKeys; )
		{
			DeleteKey( HKEY_USERS, oUKeys.GetNext( posKeys ) );
		}

		for ( POSITION posValues = oUValues.GetHeadPosition(); posValues; )
		{
			const CStringC2& pValue = oUValues.GetNext( posValues );
			DeleteValue( HKEY_USERS, pValue.first, pValue.second );
		}

		for ( POSITION posPoints = oPoints.GetHeadPosition(); posPoints; )
		{
			POSITION posCurrent = posPoints;
			CString sPoint = oPoints.GetNext( posPoints );
			CString sPointName = sPoint.Mid( sPoint.ReverseFind( _T('\\') ) + 1 );
			if ( sPointName.GetLength() == 1 &&
				// не трогать подключенные устройства
				( nDrives & ( 1 << ( sPointName.GetAt( 0 ) - _T('A') ) ) ) == 0 )
			{
				if ( bLog )
				{
					bLog = false;
					Log( IDS_RUN_CLEAN, Clean );
				}
				DeleteKey( HKEY_USERS, sPoint );
				oPoints.RemoveAt( posCurrent );
			}
		}

		for ( POSITION posVolumes = oVolumes.GetHeadPosition(); posVolumes; )
		{
			POSITION posCurrentVolumes = posVolumes;
			CString sVolume = oVolumes.GetNext( posVolumes );
			CString sVolumeName = sVolume.Mid( sVolume.ReverseFind( _T('\\') ) + 1 );
			if ( bLog )
			{
				bLog = false;
				Log( IDS_RUN_CLEAN, Clean );
			}
			for ( POSITION posPoints = oPoints.GetHeadPosition(); posPoints; )
			{
				POSITION posCurrentPoints = posPoints;
				CString sPoint = oPoints.GetNext( posPoints );
				CString sPointName = sPoint.Mid( sPoint.ReverseFind( _T('\\') ) + 1 );

				if ( sPointName == sVolumeName )
				{
					DeleteKey( HKEY_USERS, sPoint );
					oPoints.RemoveAt( posCurrentPoints );
					break;
				}
			}
			DeleteKey( HKEY_USERS, sVolume );
			oVolumes.RemoveAt( posCurrentVolumes );
		}
	}
}

void CUSBOblivionDlg::DeleteFile(LPCTSTR szPath)
{
	if ( GetFileAttributes( szPath ) != INVALID_FILE_ATTRIBUTES )
	{
		Log( LoadString( IDS_DELETE_FILE ) + szPath, Clean );
		if ( m_bEnable )
		{
			::DeleteFile( szPath );
		}
	}
}

void CUSBOblivionDlg::DeleteLog(LPCTSTR szName)
{
	if ( HANDLE hLog = OpenEventLog( NULL, szName ) )
	{
		DWORD dwCount = 0;
		if ( GetNumberOfEventLogRecords( hLog, &dwCount ) && dwCount > 1 )
		{
			Log( LoadString( IDS_RUN_LOG ) + szName, Clean );
			if ( m_bEnable )
			{
				ClearEventLog( hLog, NULL );
			}
		}
		CloseEventLog( hLog );
	}
}

void CUSBOblivionDlg::DeleteKey(HKEY hRoot, const CString& sSubKey)
{
	Log( LoadString( IDS_DELETE_KEY ) + GetKeyName( hRoot ) +
		_T("\\") + sSubKey, Clean );

	SaveKey( hRoot, sSubKey );

	RegDeleteKeyFull( hRoot, sSubKey );
}

void CUSBOblivionDlg::DeleteValue(HKEY hRoot, LPCTSTR szSubKey, LPCTSTR szValue)
{
	Log( LoadString( IDS_DELETE_VALUE ) + GetKeyName( hRoot ) +
		_T("\\") + szSubKey + _T(" -> ") + szValue, Clean );

	SaveKey( hRoot, szSubKey, szValue );

	if ( m_bEnable )
	{
		RegDeleteValueFull( hRoot, szSubKey, szValue );
	}
}

void CUSBOblivionDlg::SaveKey(HKEY hRoot, LPCTSTR szKeyName, LPCTSTR szValueName)
{
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;
	DWORD dwType;
	BYTE pszValue[ 4096 ] = {};
	DWORD cchValue;

	if ( m_oFile == INVALID_HANDLE_VALUE )
		return;

	HKEY hKey = NULL;
	LSTATUS ret = RegOpenKeyFull( hRoot, szKeyName, KEY_READ, &hKey );
	if ( ret == ERROR_SUCCESS )
	{
		Write( CString( _T("\r\n[") ) + GetKeyName( hRoot ) + _T("\\") +
			szKeyName + _T("]\r\n") );

		for ( DWORD dwIndex = 0; ; ++dwIndex )
		{
			pszName[ 0 ] = 0;
			cchName = _countof( pszName );
			dwType = 0;
			pszValue[ 0 ] = 0;
			pszValue[ 1 ] = 0;
			cchValue = sizeof( pszValue );
			ret = SHEnumValue( hKey, dwIndex, pszName, &cchName, &dwType,
				pszValue, &cchValue );
			if ( ret != ERROR_SUCCESS )
				break;
			pszValue[ cchValue ] = 0;
			pszValue[ cchValue + 1 ] = 0;
			if ( ! szValueName || CmpStrI( szValueName, pszName ) )
			{
				SaveValue( pszName, dwType, pszValue, cchValue );
				if ( szValueName )
					break;
			}
		}

		if ( ! szValueName )
		{
			for ( DWORD dwIndex = 0; ; ++dwIndex )
			{
				pszName[ 0 ] = 0;
				cchName = _countof( pszName );
				ret = SHEnumKeyEx( hKey, dwIndex, pszName, &cchName );
				if ( ret != ERROR_SUCCESS )
					break;
				SaveKey( hRoot, CString( szKeyName ) + _T("\\") + pszName );
			}
		}

		RegCloseKey( hRoot );
	}
}

void CUSBOblivionDlg::SaveValue(LPCTSTR szName, DWORD dwType, LPBYTE pData, DWORD dwSize)
{
	CString str;

	if ( m_oFile == INVALID_HANDLE_VALUE )
		return;

	if ( *szName )
	{
		Write( _T("\"") );
		Write( szName );
		Write( _T("\"=") );
	}
	else
		Write( _T("@=") );

	switch( dwType )
	{
		case REG_SZ:
			Write( _T("\"") );
			str = (LPCTSTR)pData;
			str.Replace( _T("\\"), _T("\\\\") );
			str.Replace( _T("\""), _T("\\\"") );
			Write( (LPCTSTR)str );
			Write( _T("\"") );
			break;

		case REG_DWORD:
			ASSERT( dwSize == sizeof( DWORD ) );
			str.Format( _T("dword:%08x"), *(DWORD*)pData );
			Write( str );
			break;

		case REG_BINARY:
		case REG_MULTI_SZ:
		case REG_QWORD:
			ASSERT( dwType != REG_QWORD || dwSize == sizeof( QWORD ) );
			Write( ( dwType == REG_BINARY ) ? _T("hex:") :
				( ( dwType == REG_MULTI_SZ ) ? _T("hex(7):") : _T("hex(b):") ) );
			for ( DWORD i = 0; i < dwSize; ++i )
			{
				if ( i == 0 )
					str.Format( _T("%02x"), pData[ i ] );
				else
					str.Format( _T(",%02x"), pData[ i ] );
				Write( str );
			}
			break;

		default:
			ASSERT( FALSE );
	}
	Write( _T("\r\n") );
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

	ShellExecute( GetSafeHwnd(), NULL, _T("http://www.cherubicsoft.com/projects/usboblivion"), NULL, NULL, SW_SHOWDEFAULT );

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
					msg.Format( IDS_DISK_MOUNT, i );
				else
					msg.Format( IDS_DISK_UNMOUNT, i );
				Log( msg );
			}
		}

		m_nDrives = nDrives;
	}
	return TRUE;
}
