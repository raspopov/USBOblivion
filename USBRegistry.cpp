// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// USBRegistry.cpp
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

// Keys to exclude from defs
static const CString sExcludedKeys[] =
{
	_T("Microsoft\\Windows\\Shell\\Bags\\1")
};

CString CUSBOblivionDlg::GetKeyName(HKEY hRoot)
{
	switch ( (ULONG_PTR)hRoot )
	{
	case (ULONG_PTR)HKEY_CLASSES_ROOT:
		return _T("HKEY_CLASSES_ROOT");
	case (ULONG_PTR)HKEY_CURRENT_USER:
		return _T("HKEY_CURRENT_USER");
	case (ULONG_PTR)HKEY_LOCAL_MACHINE:
		return _T("HKEY_LOCAL_MACHINE");
	case (ULONG_PTR)HKEY_USERS:
		return _T("HKEY_USERS");
	case (ULONG_PTR)HKEY_CURRENT_CONFIG:
		return _T("HKEY_CURRENT_CONFIG");
	default:
		ASSERT( 0 );
		return _T("");
	}
}

LSTATUS CUSBOblivionDlg::RegOpenKeyFull(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired, PHKEY phkResult)
{
	LSTATUS ret = RegOpenKeyEx( hKey, lpSubKey, 0, samDesired | KEY_WOW64_64KEY, phkResult );
	if ( ret != ERROR_SUCCESS )
		ret = RegOpenKeyEx( hKey, lpSubKey, 0, samDesired | KEY_WOW64_32KEY, phkResult );
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
	HKEY hValueKey = nullptr;
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
		HKEY hSubKey = nullptr;
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
		if ( theApp.dyn_RegDeleteKeyExW )
		{
			ret = theApp.dyn_RegDeleteKeyExW( hKey, sSubKey, KEY_WOW64_64KEY, 0 );
			if ( ret != ERROR_SUCCESS )
				ret = theApp.dyn_RegDeleteKeyExW( hKey, sSubKey, KEY_WOW64_32KEY, 0 );
			if ( ret != ERROR_SUCCESS )
				ret = theApp.dyn_RegDeleteKeyExW( hKey, sSubKey, 0, 0 );
		}
		else
		{
			ret = RegDeleteKeyW( hKey, sSubKey );
		}

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
	HKEY hKeys = nullptr;
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
		const CString sFullSubKey = sFullKey + _T("\\") + pszName;
		if ( ! def.szKeySubstring || StrStrI( pszName, def.szKeySubstring ) )
		{
			bool bExcluded = false;
			for ( const auto& sExcludedKey : sExcludedKeys )
			{
				const auto full_len = sFullSubKey.GetLength();
				const auto excluded_len = sExcludedKey.GetLength();
				if ( full_len >= excluded_len &&
					_tcsncicmp( (LPCTSTR)sFullSubKey + ( full_len - excluded_len ), (LPCTSTR)sExcludedKey, excluded_len ) == 0 )
				{
					// Excluded key
					bExcluded = true;
					break;
				}
			}
			if ( ! bExcluded )
			{
				if ( def.szValueName )
				{
					dwType = 0;
					pszValue[ 0 ] = 0;
					pszValue[ 1 ] = 0;
					cchValue = 4096;
					ret = SHGetValue( hKeys, pszName, def.szValueName, &dwType, pszValue, &cchValue );
					bAdd = ( ret == ERROR_SUCCESS && ( ! def.szValueSubstring || StrStrI( (LPCTSTR)pszValue, def.szValueSubstring ) ) );
				}
				else
				{
					bAdd = true;
				}
			}
		}
		if ( bAdd )
		{
			++ dwDeleted;
			AddUnique( oKeys, sFullSubKey );
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
	HKEY hKey = nullptr;
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

void CUSBOblivionDlg::DeleteKey(HKEY hRoot, const CString& sSubKey)
{
	Log( m_sDeleteKeyString + GetKeyName( hRoot ) + _T("\\") + sSubKey, Clean );

	SaveKey( hRoot, sSubKey );

	RegDeleteKeyFull( hRoot, sSubKey );
}

void CUSBOblivionDlg::DeleteValue(HKEY hRoot, LPCTSTR szSubKey, LPCTSTR szValue)
{
	Log( m_sDeleteValueString + GetKeyName( hRoot ) + _T("\\") + szSubKey + _T(" -> ") + szValue, Clean );

	SaveKey( hRoot, szSubKey, szValue );

	if ( m_bEnable )
		RegDeleteValueFull( hRoot, szSubKey, szValue );
}
