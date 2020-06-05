//
// DllLoader.h
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

template <typename FuncTypeTTT>
struct TypeOnlyStruct
{
	typedef FuncTypeTTT FuncType;
};

#define FuncPtrType(funcType)	TypeOnlyStruct<funcType>::FuncType
#define FuncLoad(x,y)			((x).LoadFunc((dyn_##y),(#y)))

class CDllLoader
{
private:
	CString		m_sDllName;
	HINSTANCE	m_pDll;

public:
	inline CDllLoader() noexcept
		: m_pDll ( nullptr )
	{
	}

	inline CDllLoader(LPCTSTR szDllName, bool bloadNow = true)
		: m_sDllName	( szDllName )
		, m_pDll		( nullptr )
	{
		if ( bloadNow )
		{
			LoadLibrary();
		}
	}

	~CDllLoader()
	{
		Free();
	}

	inline bool Load(LPCTSTR szDllName)
	{
		// existing lib loaded?
		if ( m_pDll )
			return true;

		m_sDllName = szDllName;

		return LoadLibrary();
	}

	inline operator bool() const noexcept
	{
		return ( m_pDll != nullptr );
	}

	template < typename FuncTTT >
	FuncTTT LoadFunc(FuncTTT& c, LPCSTR fNameStr)
	{
		// existing lib loaded?
		if ( ! LoadLibrary() )
			return (FuncTTT)nullptr;

		// load func from dll
		FuncTTT fPtr = (FuncTTT)GetProcAddress ( m_pDll, fNameStr );
		if ( ! fPtr )
		{
			ATLTRACE( "ERROR: cannot locate function name (%s) from dll\n", fNameStr );
			return (FuncTTT)nullptr;
		}
		c = fPtr;
		return fPtr;
	}

	void Free() noexcept
	{
		if ( m_pDll )
		{
			ATLTRACE( _T("Unloading DLL %s ...\n"), (LPCTSTR)m_sDllName );
			ATLVERIFY( ::FreeLibrary( m_pDll ) );
			ATLTRACE( _T("DLL %s successfully unloaded.\n"), (LPCTSTR)m_sDllName );
			m_pDll = nullptr;
		}
	}

	bool LoadLibrary(HINSTANCE hInstance = nullptr)
	{
		// existing lib loaded?
		if ( m_pDll )
			return true;

		// load from:
		// 1. The directory from which the application loaded.
		// 2. The current directory.
		// 3. The Windows system directory.
		// 4. The Windows directory.
		// 5. The directories that are listed in the PATH environment variable.
		ATLTRACE( _T("Trying LoadLibrary %s ...\n"), (LPCTSTR)m_sDllName );
		m_pDll = ::LoadLibrary( m_sDllName );
		if ( m_pDll )
			return true;

		// 6. The module directory (if dll).
		if ( hInstance )
		{
			CString self;
			GetModuleFileName( hInstance, self.GetBuffer( MAX_PATH + 1 ), MAX_PATH );
			self.ReleaseBuffer ();
			self = self.Mid( 0, self.ReverseFind( _T('\\') ) + 1 );
			m_sDllName = self + m_sDllName;

			ATLTRACE( _T("Trying LoadLibrary %s ...\n"), (LPCTSTR)m_sDllName );
			m_pDll = ::LoadLibrary( m_sDllName );
			if ( m_pDll )
				return true;
		}

		ATLTRACE( _T("ERROR: cannot load library %s\n"), (LPCTSTR)m_sDllName );
		return false;
	}
};
