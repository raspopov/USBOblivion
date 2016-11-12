//
// USBOblivion.cpp
//
// Copyright (c) Nikolay Raspopov, 2009-2016.
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

#include "Localization\Localization.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LOCALE_INVARIANT_W2K \
	(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT))

BEGIN_MESSAGE_MAP(CUSBOblivionApp, CWinApp)
END_MESSAGE_MAP()

CUSBOblivionApp theApp;

CUSBOblivionApp::CUSBOblivionApp()
{
}

BOOL CUSBOblivionApp::InitInstance()
{
	VERIFY( SUCCEEDED( CoInitializeEx( 0, COINIT_MULTITHREADED ) ) );

	const INITCOMMONCONTROLSEX InitCtrls = { sizeof( INITCOMMONCONTROLSEX ), ICC_WIN95_CLASSES };
	InitCommonControlsEx( &InitCtrls );

	CWinApp::InitInstance();

	SetRegistryKey( AfxGetAppName() );

	CUSBOblivionDlg dlg;

	LANGID nLang = 0;
	BOOL bHelp = FALSE;
	int nArgs = 0;
	LPWSTR* szArglist = CommandLineToArgvW( GetCommandLineW(), &nArgs );
	if ( szArglist )
	{
		for( int i = 1; i < nArgs; ++i )
		{
			if ( CmpStrI( szArglist[ i ], _T("-?") ) || CmpStrI( szArglist[ i ], _T("/?") ) )
			{
				bHelp = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-enable") ) || CmpStrI( szArglist[ i ], _T("/enable") ) )
			{
				dlg.m_bEnable = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-auto") ) || CmpStrI( szArglist[ i ], _T("/auto") ) )
			{
				dlg.m_bAuto = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-norestorepoint") ) || CmpStrI( szArglist[ i ], _T("/norestorepoint") ) )
			{
				dlg.m_bRestorePoint = FALSE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-nosave") ) || CmpStrI( szArglist[ i ], _T("/nosave") ) )
			{
				if ( dlg.m_sSave.IsEmpty() )
					dlg.m_bSave = FALSE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-elevation") ) || CmpStrI( szArglist[ i ], _T("/elevation") ) )
			{
				dlg.m_bElevation = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-silent") ) || CmpStrI( szArglist[ i ], _T("/silent") ) )
			{
				dlg.m_bSilent = TRUE;
			}
			else if ( CmpStrI( szArglist[ i ], _T("-lang:"), 6 ) || CmpStrI( szArglist[ i ], _T("/lang:"), 6 ) )
			{
				int lang;
				if ( _stscanf_s( szArglist[ i ] + 6, _T("%x"), &lang ) == 1 )
				{
					nLang = (LANGID)lang;
				}
			}
			else if ( CmpStrI( szArglist[ i ], _T("-log:"), 5 ) || CmpStrI( szArglist[ i ], _T("/log:"), 5 ) )
			{
				dlg.m_sLog = szArglist[ i ] + 5;
				dlg.m_sLog.Trim( _T(" \t\r\n\"") );
			}
			else if ( CmpStrI( szArglist[ i ], _T("-save:"), 6 ) || CmpStrI( szArglist[ i ], _T("/save:"), 6 ) )
			{
				dlg.m_sSave = szArglist[ i ] + 6;
				dlg.m_sSave.Trim( _T(" \t\r\n\"") );
				if ( ! dlg.m_sSave.IsEmpty() )
					dlg.m_bSave = TRUE;
			}
		}
	}
	LocalFree( szArglist );

	m_Loc.Load();
	m_Loc.Select( nLang );

	if ( bHelp )
	{
		AfxMessageBox( LoadString( IDS_ABOUT ) );
	}
	else
	{
		m_pMainWnd = &dlg;
		dlg.DoModal();
	}

	return FALSE;
}

int CUSBOblivionApp::ExitInstance()
{
	CWinApp::ExitInstance();

	CoUninitialize();

	// Всегда возврат 0, для нормального запуска через другие программы
	return 0;
}

/*HMODULE AfxLoadSystemLibraryUsingFullPath(_In_z_ const WCHAR *pszLibrary)
{
WCHAR wszLoadPath[ MAX_PATH + 1 ];
if ( ::GetSystemDirectoryW( wszLoadPath, _countof( wszLoadPath ) ) == 0 )
{
return NULL;
}

if ( wszLoadPath[ wcslen( wszLoadPath ) - 1 ] != L'\\' )
{
if ( wcscat_s( wszLoadPath, _countof( wszLoadPath ), L"\\") != 0 )
{
return NULL;
}
}

if ( wcscat_s( wszLoadPath, _countof( wszLoadPath ), pszLibrary ) != 0 )
{
return NULL;
}

return ( ::AfxCtxLoadLibraryW( wszLoadPath ) );
}*/

bool CmpStrI(LPCTSTR szLeft, LPCTSTR szRight, int nCount)
{
	int ret = CompareString( LOCALE_INVARIANT, NORM_IGNORECASE,
		szLeft, nCount, szRight, nCount );
	if ( ! ret )
		ret = CompareString( LOCALE_INVARIANT_W2K, NORM_IGNORECASE,
			szLeft, nCount, szRight, nCount );
	ASSERT( ret );
	return ( ret == CSTR_EQUAL );
}

bool IfNotExist(const CStringList& lst, const CString& str)
{
	CString sRoot;
	for ( int i = 0; ; )
	{
		CString sToken = str.Tokenize( _T("\\"), i );
		if ( sToken.IsEmpty() )
			break;
		if ( ! sRoot.IsEmpty() )
			sRoot += _T("\\");
		sRoot += sToken;
		if ( lst.Find( sRoot ) )
			return false;
	}
	return true;
}

void AddUnique(CStringList& lst, CString str)
{
	str.MakeLower();
	if ( IfNotExist( lst, str ) )
		lst.AddTail( str );
}

BOOL IsRunAsAdmin()
{
	PSID pAdministratorsGroup = NULL;
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if ( ! AllocateAndInitializeSid( &NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdministratorsGroup ) )
	{
		TRACE( _T("AllocateAndInitializeSid error: %d\n"), GetLastError() );
		return FALSE;
	}

	BOOL bIsRunAsAdmin = FALSE;
	if ( ! CheckTokenMembership( NULL, pAdministratorsGroup, &bIsRunAsAdmin ) )
	{
		TRACE( _T("CheckTokenMembership error: %d\n"), GetLastError() );
		FreeSid( pAdministratorsGroup );
		return FALSE;
	}

	FreeSid( pAdministratorsGroup );
	return bIsRunAsAdmin;
}

BOOL IsProcessElevated()
{
	HANDLE hToken = NULL;
	if ( ! OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken ) )
	{
		TRACE( _T("OpenProcessToken error: %d\n"), GetLastError() );
		return FALSE;
	}

	TOKEN_ELEVATION elevation = {};
	DWORD dwSize = 0;
	if ( ! GetTokenInformation( hToken, TokenElevation, &elevation, 
		sizeof( elevation ), &dwSize ) )
	{
		TRACE( _T("GetTokenInformation error: %d\n"), GetLastError() );
		CloseHandle( hToken );
		return FALSE;
	}

	CloseHandle( hToken );
	return ( elevation.TokenIsElevated != FALSE );
}

HANDLE OpenProcessToken(HANDLE hProcess, DWORD dwAccess)
{
	// Вначале открытие токена "по хорошему"
	HANDLE hToken = NULL;
	if ( OpenProcessToken( hProcess, dwAccess, &hToken ) )
		return hToken;

	// Теперь будем переписывать список доступа токена...

	// Получение своего SIDа из токена своего процесса
	HANDLE hSelfProcess = GetCurrentProcess();
	HANDLE hSelfToken = NULL;
	if ( OpenProcessToken( hSelfProcess, TOKEN_READ, &hSelfToken ) )
	{
		BYTE pBuf[ 512 ] = {};
		DWORD dwLen = sizeof( pBuf );
		if ( GetTokenInformation( hSelfToken, TokenUser, pBuf, dwLen, &dwLen ) )
		{
			// Сборка дополнительного члена DACL, где своему SIDу разрешён нужный доступ
			EXPLICIT_ACCESS ea = 
			{
				dwAccess,
				GRANT_ACCESS,
				NO_INHERITANCE,
				{
					NULL,
					NO_MULTIPLE_TRUSTEE,
					TRUSTEE_IS_SID,
					TRUSTEE_IS_USER,
					(LPTSTR)( (TOKEN_USER*)pBuf )->User.Sid
				}
			};

			// Открытие токена процесса с правами перезаписи DACL
			// (нужна соответствующая привелегия)
			if ( OpenProcessToken( hProcess, TOKEN_READ | WRITE_OWNER |
				WRITE_DAC, &hToken ) )
			{
				// Получение текущего DACL токена
				PACL pOrigDacl = NULL;
				PSECURITY_DESCRIPTOR pSD = NULL;
				if ( GetSecurityInfo( hToken, SE_KERNEL_OBJECT,
					DACL_SECURITY_INFORMATION, NULL, NULL,
					&pOrigDacl, NULL, &pSD ) == ERROR_SUCCESS )
				{
					// Добавление себя в DACL токена
					PACL pNewDacl = NULL;
					if ( SetEntriesInAcl( 1, &ea, pOrigDacl, &pNewDacl ) == ERROR_SUCCESS )
					{
						// Установка нового DACL токена (перманентно!)
						if ( SetSecurityInfo( hToken, SE_KERNEL_OBJECT,
							DACL_SECURITY_INFORMATION, NULL, NULL,
							pNewDacl, NULL ) == ERROR_SUCCESS )
						{
							// Переоткрытие хэндла с нужными правами
							if ( DuplicateHandle( hSelfProcess, hToken, hSelfProcess,
								&hToken, dwAccess, FALSE, DUPLICATE_CLOSE_SOURCE ) )
							{
								TRACE( _T("Got it!\n") );
							}
							else
								hToken = NULL;
						}
						LocalFree( pNewDacl );
					}
					LocalFree( pSD );
				}
			}
		}
		CloseHandle( hSelfToken );
	}
	return hToken;
}

DEVINST GetDrivesDevInstByDeviceNumber(DWORD DeviceNumber, UINT DriveType, LPCTSTR szDosDeviceName)
{
	GUID* guid;
	switch ( DriveType )
	{
	case DRIVE_REMOVABLE:
		if ( StrStrI( szDosDeviceName, _T("\\Floppy") ) != NULL )
			guid = (GUID*)&GUID_DEVINTERFACE_FLOPPY;
		else
			guid = (GUID*)&GUID_DEVINTERFACE_DISK;
		break;

	case DRIVE_FIXED:
		guid = (GUID*)&GUID_DEVINTERFACE_DISK;
		break;

	case DRIVE_CDROM:
		guid = (GUID*)&GUID_DEVINTERFACE_CDROM;
		break;

	default:
		return 0;
	}

	// Get device interface info set handle for all devices attached to system
	HDEVINFO hDevInfo = SetupDiGetClassDevs( guid, NULL, NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
	if ( hDevInfo == INVALID_HANDLE_VALUE )
		return 0;

	// Retrieve a context structure for a device interface of a device information set
	for ( DWORD dwIndex = 0; ; ++dwIndex )
	{
		SP_DEVICE_INTERFACE_DATA spdid = { sizeof( SP_DEVICE_INTERFACE_DATA ) };
		if ( ! SetupDiEnumDeviceInterfaces( hDevInfo, NULL, guid, dwIndex, &spdid ) )
			break;

		// check the buffer size
		DWORD dwSize = 0;
		SetupDiGetDeviceInterfaceDetail( hDevInfo, &spdid, NULL, 0, &dwSize, NULL );

		if ( dwSize != 0 )
		{
			BYTE* pBuf = new BYTE[ dwSize + 1 ];
			if ( pBuf )
			{
				ZeroMemory( pBuf, dwSize + 1 );
				PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd =
					(PSP_DEVICE_INTERFACE_DETAIL_DATA)pBuf;
				pspdidd->cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );
				SP_DEVINFO_DATA spdd = { sizeof( SP_DEVINFO_DATA ) };
				if ( SetupDiGetDeviceInterfaceDetail( hDevInfo, &spdid, pspdidd,
					dwSize, &dwSize, &spdd ) )
				{
					// in case you are interested in the USB serial number:
					// the device id string contains the serial number
					// if the device has one, otherwise a generated id
					// that contains the '&' char...

					//DEVINST DevInstParent = 0;
					//CM_Get_Parent(&DevInstParent, spdd.DevInst, 0); 
					//char szDeviceIdString[MAX_PATH];
					//CM_Get_Device_ID(DevInstParent, szDeviceIdString, MAX_PATH, 0);
					//printf("DeviceId=%s\n", szDeviceIdString);

					// open the disk or cd-rom or floppy
					HANDLE hDrive = CreateFile( pspdidd->DevicePath, 0,
						FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
					if ( hDrive != INVALID_HANDLE_VALUE )
					{
						// get its device number
						STORAGE_DEVICE_NUMBER sdn = {};
						DWORD dwBytesReturned = 0;
						if ( DeviceIoControl( hDrive, IOCTL_STORAGE_GET_DEVICE_NUMBER,
							NULL, 0, &sdn, sizeof( sdn ), &dwBytesReturned, NULL ) )
						{
							if ( DeviceNumber == sdn.DeviceNumber )
							{
								// match the given device number with the
								// one of the current device
								CloseHandle( hDrive );
								delete [] pBuf;
								SetupDiDestroyDeviceInfoList( hDevInfo );
								return spdd.DevInst;
							}
						}
						CloseHandle( hDrive );
					}
				}
				delete [] pBuf;
			}
		}
	}
	SetupDiDestroyDeviceInfoList( hDevInfo );
	return 0;
}

/*
BOOL ClearLog(LPCWSTR szLog = L"Microsoft-Windows-DriverFrameworks-UserMode/Operational" )
{
CComPtr< IWbemLocator > pLoc;
HRESUILT hr = pLoc.CoCreateInstance( CLSID_WbemLocator );
if ( SUCCEEDED( hr ) && pLoc )
{
CComPtr< IWbemServices > pSvc;
hr = pLoc->ConnectServer(
CComBSTR(L"ROOT\\CIMV2"), // Object path of WMI namespace
NULL,                    // User name. NULL = current user
NULL,                    // User password. NULL = current
0,                       // Locale. NULL indicates current
NULL,                    // Security flags.
0,                       // Authority (for example, Kerberos)
0,                       // Context object 
&pSvc                    // pointer to IWbemServices proxy
);
if ( SUCCEEDED( hr ) && pSvc )
{
hr = CoSetProxyBlanket(
pSvc,                        // Indicates the proxy to set
RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
NULL,                        // Server principal name 
RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
NULL,                        // client identity
EOAC_NONE                    // proxy capabilities 
);

CComPtr< IEnumWbemClassObject > pEnumerator;
hr = pSvc->ExecQuery(
CComBSTR( L"WQL" ), 
CComBSTR( CStringW( L"SELECT * FROM Win32_NTEventLogFile WHERE LogFileName=\"" ) + szLog + L"\"" ),
WBEM_FLAG_BIDIRECTIONAL | WBEM_FLAG_RETURN_IMMEDIATELY, 
NULL,
&pEnumerator );
if ( SUCCEEDED( hr ) && pEnumerator )
{
for ( ;; )
{
CComPtr< IWbemClassObject > pclsObj;
ULONG uReturn = 0;
hr = pEnumerator->Next( WBEM_INFINITE, 1, &pclsObj, &uReturn );
if ( 0 == uReturn || ! pclsObj )
break;

CComVariant vtPath;
hr = pclsObj->Get( L"__Path", 0, &vtPath, NULL, NULL );
if ( SUCCEEDED( hr ) )
{
hr = pSvc->ExecMethod( vtPath.bstrVal, CComBSTR( L"ClearEventLog" ), 0, NULL, NULL, NULL, NULL );
}
}
}
}
}
}
*/

BOOL InitializeCOMSecurity()
{   
	// Create the security descriptor explicitly as follows because
	// CoInitializeSecurity() will not accept the relative security descriptors  
	// returned by ConvertStringSecurityDescriptorToSecurityDescriptor().
	SECURITY_DESCRIPTOR securityDesc = {};
	EXPLICIT_ACCESS   ea[5] = {};
	ACL        *pAcl = NULL;
	ULONGLONG  rgSidBA[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={};
	ULONGLONG  rgSidLS[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={};
	ULONGLONG  rgSidNS[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={};
	ULONGLONG  rgSidPS[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={};
	ULONGLONG  rgSidSY[(SECURITY_MAX_SID_SIZE+sizeof(ULONGLONG)-1)/sizeof(ULONGLONG)]={};
	DWORD      cbSid = 0;
	DWORD      dwRet = ERROR_SUCCESS;
	HRESULT    hrRet = S_OK;

	// Initialize the security descriptor.
	BOOL fRet = ::InitializeSecurityDescriptor( &securityDesc, SECURITY_DESCRIPTOR_REVISION );
	if( !fRet )
	{
		goto exit;
	}

	// Create an administrator group security identifier (SID).
	cbSid = sizeof( rgSidBA );
	fRet = ::CreateWellKnownSid( WinBuiltinAdministratorsSid, NULL, rgSidBA, &cbSid );
	if( !fRet )
	{
		goto exit;
	}

	// Create a local service security identifier (SID).
	cbSid = sizeof( rgSidLS );
	fRet = ::CreateWellKnownSid( WinLocalServiceSid, NULL, rgSidLS, &cbSid );
	if( !fRet )
	{
		goto exit;
	}

	// Create a network service security identifier (SID).
	cbSid = sizeof( rgSidNS );
	fRet = ::CreateWellKnownSid( WinNetworkServiceSid, NULL, rgSidNS, &cbSid );
	if( !fRet )
	{
		goto exit;
	}

	// Create a personal account security identifier (SID).
	cbSid = sizeof( rgSidPS );
	fRet = ::CreateWellKnownSid( WinSelfSid, NULL, rgSidPS, &cbSid );
	if( !fRet )
	{
		goto exit;
	}

	// Create a local service security identifier (SID).
	cbSid = sizeof( rgSidSY );
	fRet = ::CreateWellKnownSid( WinLocalSystemSid, NULL, rgSidSY, &cbSid );
	if( !fRet )
	{
		goto exit;
	}

	// Setup the access control entries (ACE) for COM

	ea[0].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.pMultipleTrustee = NULL;
	ea[0].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)rgSidBA;

	ea[1].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.pMultipleTrustee = NULL;
	ea[1].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR)rgSidLS;

	ea[2].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[2].grfAccessMode = SET_ACCESS;
	ea[2].grfInheritance = NO_INHERITANCE;
	ea[2].Trustee.pMultipleTrustee = NULL;
	ea[2].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[2].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[2].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[2].Trustee.ptstrName = (LPTSTR)rgSidNS;

	ea[3].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[3].grfAccessMode = SET_ACCESS;
	ea[3].grfInheritance = NO_INHERITANCE;
	ea[3].Trustee.pMultipleTrustee = NULL;
	ea[3].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[3].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[3].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[3].Trustee.ptstrName = (LPTSTR)rgSidPS;

	ea[4].grfAccessPermissions = COM_RIGHTS_EXECUTE | COM_RIGHTS_EXECUTE_LOCAL;
	ea[4].grfAccessMode = SET_ACCESS;
	ea[4].grfInheritance = NO_INHERITANCE;
	ea[4].Trustee.pMultipleTrustee = NULL;
	ea[4].Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	ea[4].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[4].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[4].Trustee.ptstrName = (LPTSTR)rgSidSY;

	// Create an access control list (ACL) using this ACE list.
	dwRet = ::SetEntriesInAcl( ARRAYSIZE( ea ), ea, NULL, &pAcl );
	if( dwRet != ERROR_SUCCESS || pAcl == NULL )
	{
		fRet = FALSE;
		goto exit;
	}

	// Set the security descriptor owner to Administrators.
	fRet = ::SetSecurityDescriptorOwner( &securityDesc, rgSidBA, FALSE );
	if( !fRet )
	{
		goto exit;
	}

	// Set the security descriptor group to Administrators.
	fRet = ::SetSecurityDescriptorGroup( &securityDesc, rgSidBA, FALSE );
	if( !fRet )
	{
		goto exit;
	}

	// Set the discretionary access control list (DACL) to the ACL.
	fRet = ::SetSecurityDescriptorDacl( &securityDesc, TRUE, pAcl, FALSE );
	if( !fRet )
	{
		goto exit;
	}

	// Initialize COM
	hrRet = ::CoInitializeSecurity( &securityDesc,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
		RPC_C_IMP_LEVEL_IDENTIFY,
		NULL,
		EOAC_DISABLE_AAA | EOAC_NO_CUSTOM_MARSHAL,
		NULL );
	if( FAILED( hrRet ) && hrRet != RPC_E_TOO_LATE )
	{
		fRet = FALSE;
		goto exit;
	}

	fRet = TRUE;

exit:
	::LocalFree( pAcl );

	return fRet;
}
