//
// stdafx.h
//
// Copyright (c) Nikolay Raspopov, 2009-2012.
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

#pragma once

#pragma warning(disable: 4201)

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define WIN32_LEAN_AND_MEAN

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_NO_COM_SUPPORT
#define _AFX_ALL_WARNINGS
#define NO_PRINT

#include "miniVS2010.h"

#include <afxwin.h>
#include <afxcmn.h>
#include <afxtempl.h>
#include <atlsecurity.h>
#include <atlcoll.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <shlobj.h>
//#include <dbt.h>
#include <setupapi.h>
#include <winioctl.h>
#include <cfgmgr32.h>

#include "thread.h"
#include "Localization\Localization.h"

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


// Сравнение строк без учёта регистра и локали
bool CmpStrI(LPCTSTR szLeft, LPCTSTR szRight, int nCount = -1);

// Проверка наличия строки в списке строк
bool IfNotExist(const CStringList& lst, const CString& str);

// Добавление строки в список строк без дубликатов
void AddUnique(CStringList& lst, CString str);

// Определение наличия административных прав своего процесса
BOOL IsRunAsAdmin();

// Определение наличия повышенных прав (UAC) своего процесса
BOOL IsProcessElevated();

// Открытие токена процесса с гарантированными правами
HANDLE OpenProcessToken(HANDLE hProcess, DWORD dwAccess);

// Возвращает хэндлер дискового устройства по его номеру (Uwe Sieber - www.uwe-sieber.de)
DEVINST GetDrivesDevInstByDeviceNumber(DWORD DeviceNumber, UINT DriveType, LPCTSTR szDosDeviceName);

using namespace c4u;
