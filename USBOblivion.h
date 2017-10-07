//
// USBOblivion.h
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

#include "resource.h"

#include "thread.h"
#include "Localization\Localization.h"
#include "DllLoader.h"

using namespace c4u;


class CUSBOblivionApp : public CWinApp
{
public:
	CUSBOblivionApp();

	CLocalization	m_Loc;

	CDllLoader	m_advapi32;
	FuncPtrType( LSTATUS ( WINAPI * )( _In_ HKEY hKey, _In_ LPCWSTR lpSubKey, _In_ REGSAM samDesired, _Reserved_ DWORD Reserved ) ) dyn_RegDeleteKeyExW;

	CDllLoader	m_srclient;
	FuncPtrType( BOOL  ( WINAPI * )( _In_ PRESTOREPOINTINFOW pRestorePtSpec, _Out_ PSTATEMGRSTATUS pSMgrStatus ) ) dyn_SRSetRestorePointW;

	CDllLoader	m_rstrtmgr;
	FuncPtrType( DWORD ( WINAPI * )( _Out_ DWORD *pSessionHandle, _Reserved_ DWORD dwSessionFlags, _Out_writes_( CCH_RM_SESSION_KEY + 1 ) WCHAR strSessionKey[] ) ) dyn_RmStartSession;
	FuncPtrType( DWORD ( WINAPI * )( _In_ DWORD dwSessionHandle ) ) dyn_RmEndSession;
	FuncPtrType( DWORD ( WINAPI * )( _In_ DWORD dwSessionHandle, _In_ UINT nFiles, _In_reads_opt_( nFiles ) LPCWSTR rgsFileNames[], _In_ UINT nApplications, _In_reads_opt_( nApplications ) RM_UNIQUE_PROCESS rgApplications[], _In_ UINT nServices, _In_reads_opt_( nServices ) LPCWSTR rgsServiceNames[] ) ) dyn_RmRegisterResources;
	FuncPtrType( DWORD ( WINAPI * )( _In_ DWORD dwSessionHandle, _In_ ULONG lActionFlags, _In_opt_ RM_WRITE_STATUS_CALLBACK fnStatus ) ) dyn_RmShutdown;

	CDllLoader	m_psapi;
	FuncPtrType( BOOL  ( WINAPI * )( _Out_writes_bytes_( cb ) DWORD * lpidProcess, _In_ DWORD cb, _Out_ LPDWORD lpcbNeeded ) ) dyn_EnumProcesses;
	FuncPtrType( DWORD ( WINAPI * )( _In_ HANDLE hProcess, _Out_writes_( nSize ) LPWSTR lpImageFileName, _In_ DWORD nSize ) ) dyn_GetProcessImageFileNameW;

protected:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CUSBOblivionApp theApp;

// Загрузка строки из ресурса
inline CString LoadString(UINT nID)
{
	return theApp.m_Loc.LoadString( nID );
}

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

BOOL InitializeCOMSecurity();
