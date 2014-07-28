//
// stdafx.cpp
//
// Copyright (c) Nikolay Raspopov, 2009-2014.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Localization\Localization.cpp"


#define LOCALE_INVARIANT_W2K \
	(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT))

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
