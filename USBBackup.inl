//
// USBBackup.inl
//
// Copyright (c) Nikolay Raspopov, 2009-2017.
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

#pragma once

bool CUSBOblivionDlg::PrepareBackup()
{
	if ( !m_bSave )
		return true;

	CString sPath = m_sSave;

	if ( sPath.IsEmpty() )
	{
		CString sComputer;
		DWORD dwComputerLen = MAX_PATH;
		GetComputerName( sComputer.GetBuffer( MAX_PATH + 1 ), &dwComputerLen );
		sComputer.ReleaseBuffer();

		CString sLogFolder;
		SHGetFolderPath( GetSafeHwnd(), CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT,
			sLogFolder.GetBuffer( MAX_PATH + 1 ) );
		sLogFolder.ReleaseBuffer();
		sLogFolder.TrimRight( _T( "\\" ) );

		sPath.Format(
#ifdef WIN64
			_T( "%s\\USBOblivion-64-%s-%s.reg" ),
#else
			_T( "%s\\USBOblivion-32-%s-%s.reg" ),
#endif
			(LPCTSTR)sLogFolder, (LPCTSTR)sComputer, (LPCTSTR)CTime::GetCurrentTime().Format( _T( "%y%m%d-%H%M%S" ) ) );
	}

	if ( !m_oFile.Open( sPath, CFile::modeCreate | CFile::modeWrite ) )
	{
		Log( LoadString( IDS_ERROR_FILE_CREATE ) + sPath, Error );
		return false;
	}

	try
	{
		Write( _T( "\xfeffWindows Registry Editor Version 5.00\r\n" ) );
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
	if ( m_oFile == INVALID_HANDLE_VALUE )
		return;

	m_oFile.Close();
}

void CUSBOblivionDlg::Write( LPCTSTR szText )
{
	if ( m_oFile == INVALID_HANDLE_VALUE )
		return;

	m_oFile.Write( szText, (UINT)( _tcslen( szText ) * sizeof( TCHAR ) ) );
}

void CUSBOblivionDlg::SaveKey( HKEY hRoot, LPCTSTR szKeyName, LPCTSTR szValueName )
{
	if ( m_oFile == INVALID_HANDLE_VALUE )
		return;

	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;
	DWORD dwType;
	BYTE pszValue[ 4096 ] = {};
	DWORD cchValue;

	HKEY hKey = nullptr;
	LSTATUS ret = RegOpenKeyFull( hRoot, szKeyName, KEY_READ, &hKey );
	if ( ret == ERROR_SUCCESS )
	{
		Write( CString( _T( "\r\n[" ) ) + GetKeyName( hRoot ) + _T( "\\" ) +
			szKeyName + _T( "]\r\n" ) );

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
			if ( !szValueName || CmpStrI( szValueName, pszName ) )
			{
				SaveValue( pszName, dwType, pszValue, cchValue );
				if ( szValueName )
					break;
			}
		}

		if ( !szValueName )
		{
			for ( DWORD dwIndex = 0; ; ++dwIndex )
			{
				pszName[ 0 ] = 0;
				cchName = _countof( pszName );
				ret = SHEnumKeyEx( hKey, dwIndex, pszName, &cchName );
				if ( ret != ERROR_SUCCESS )
					break;
				SaveKey( hRoot, CString( szKeyName ) + _T( "\\" ) + pszName );
			}
		}

		RegCloseKey( hRoot );
	}
}

void CUSBOblivionDlg::SaveValue( LPCTSTR szName, DWORD dwType, LPBYTE pData, DWORD dwSize )
{
	CString str;

	if ( m_oFile == INVALID_HANDLE_VALUE )
		return;

	if ( *szName )
	{
		Write( _T( "\"" ) );
		Write( szName );
		Write( _T( "\"=" ) );
	}
	else if ( dwType != REG_SZ || dwSize != 0 )
	{
		Write( _T( "@=" ) );
	}

	switch ( dwType )
	{
	case REG_SZ:
		Write( _T( "\"" ) );
		str = (LPCTSTR)pData;
		str.Replace( _T( "\\" ), _T( "\\\\" ) );
		str.Replace( _T( "\"" ), _T( "\\\"" ) );
		Write( (LPCTSTR)str );
		Write( _T( "\"" ) );
		break;

	case REG_DWORD:
		ASSERT( dwSize == sizeof( DWORD ) );
		str.Format( _T( "dword:%08x" ), *(DWORD*)pData );
		Write( str );
		break;

	case REG_NONE:
		ASSERT( dwSize == 0 );
		Write( _T( "hex(0):" ) );
		break;

	default:
		switch ( dwType )
		{
		case REG_BINARY:
			Write( _T( "hex:" ) );
			break;
		case REG_EXPAND_SZ:
			Write( _T( "hex(2):" ) );
			break;
		case REG_MULTI_SZ:
			Write( _T( "hex(7):" ) );
			break;
		case REG_QWORD:
			ASSERT( dwSize == sizeof( QWORD ) );
			Write( _T( "hex(b):" ) );
			break;
		default:
			str.Format( _T( "hex(%08x):" ), dwType );
			Write( str );
		}
		for ( DWORD i = 0; i < dwSize; ++i )
		{
			if ( i == 0 )
				str.Format( _T( "%02x" ), pData[ i ] );
			else
				str.Format( _T( ",%02x" ), pData[ i ] );
			Write( str );
		}
	}
	Write( _T( "\r\n" ) );
}
