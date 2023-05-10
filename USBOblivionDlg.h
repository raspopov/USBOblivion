//
// USBOblivionDlg.h
//
// Copyright (c) Nikolay Raspopov, 2009-2023.
// This file is part of USB Oblivion (https://www.cherubicsoft.com/en/projects/usboblivion/)
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

#include "CtrlsResize.h"


using CKeyType = enum
{
	mControlSet_Key,// ����� ������   � HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*
	mControlSet_Val,// ����� �������� � HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*
	mHKLM_Key,		// ����� ������   � HKEY_LOCAL_MACHINE
	mHKLM_Val,		// ����� �������� � HKEY_LOCAL_MACHINE
	mHKCU_Key,		// ����� ������   � HKEY_USERS\*
	mHKCU_Val,		// ����� �������� � HKEY_USERS\*
	mHKCR_Key,		// ����� ������   � HKEY_CLASSES_ROOT
	mHKCR_Val		// ����� �������� � HKEY_CLASSES_ROOT
};

using CKeyDef = struct
{
	CKeyType	nMode;				// ��� �����
	LPCTSTR		szKeyName;			// ������� ������ ����� (NULL - �����)
	LPCTSTR		szKeySubstring;		// ��� ������ ������ ����������� � ����� �����/�������� (NULL - �����)
	LPCTSTR		szValueName;		// �������� ����������� ������� ����� (NULL - �� ���������)
	LPCTSTR		szValueSubstring;	// ��� ������ ������ ����������� � �������� ����� (NULL - �����)
	BOOL		bDeleteEmpty;		// ������� ��� ���� ���� �� ������
};

template < class T1, class T2 >
class C2
{
public:
	inline C2() noexcept = default;
	inline C2(const T1& f, const T2& s) noexcept : first( f ), second( s ) {}
	T1 first;
	T2 second;
};

using CStringC2 = C2 < CString, CString >;

using CStringC2List = CList < CStringC2 >;


class CUSBOblivionDlg : public CDialog
{
public:
	CUSBOblivionDlg(CWnd* pParent = nullptr) noexcept;
	virtual ~CUSBOblivionDlg();

	enum { IDD = IDD_USBOBLIVION_DIALOG };

	BOOL		m_bEnable;			// -enable         - Do real clean (simulation otherwise)
	BOOL		m_bAuto;			// -auto           - Automatic run
	BOOL		m_bSave;			// -nosave         - Don't save a registry backup file
	CString		m_sSave;			// -save:filename  - Save a registry backup to this file
	CString		m_sLog;				// -log:filename   - Save working log to this file
	BOOL		m_bRestorePoint;	// -norestorepoint - Don't create a System Restore Point
	BOOL		m_bReboot;			// -norestart      - Don't restart Windows
	BOOL		m_bCloseExplorer;	// -noexplorer     - Don't close Windows Explorer
	BOOL		m_bSilent;			// -silent         - Hidden mode (if possible)

	BOOL		m_bElevation;		// -elevation

	INT_PTR DoModal() override;

protected:
	CImageList	m_oImages;		// ������ ������
	HICON		m_hIcon;		// ������ ����
	CListCtrl	m_pReport;		// �����
	int			m_nSelected;	// ������� ��������� ������ � ������
	CStdioFile	m_oFile;		// ���� �� .reg-�����
	CCtrlResize	m_CtrlsResize;	// ��������� �������� ����������
	CRect		m_InitialRect;	// ��������� ������� ���� - ����������� �������
	DWORD		m_nDrives;		// ������� �����

	CString		m_sDeleteKeyString;		// ��� ������
	CString		m_sDeleteValueString;	// ��� ������

	using CLogItem = C2< CString, UINT >;
	using CLogList = CList< CLogItem* >;
	cs						m_pSection;		// ������������� ������� � ������
	CAutoPtr< CLogList >	m_pReportList;	// ������ ����� ��� ������
	bool					m_bRunning;		// ���� ���������

	enum { Information = 0, Warning, Error, Search, Done, Clean, Regedit, Lock, Eject };

	void Log(const CString& sText, UINT nType = Information);
	void Log(UINT nID, UINT nType = Information);

	static CString GetKeyName(HKEY hRoot);
	LSTATUS RegOpenKeyFull(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired, PHKEY phkResult);
	LSTATUS RegDeleteKeyFull(HKEY hKey, const CString& sSubKey);
	LSTATUS RegDeleteValueFull(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue);

	void ProcessKey(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringList& oKeys);
	void ProcessValue(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringC2List& oValues);

	// �������� .reg-�����
	bool PrepareBackup();
	// �������� .reg-�����
	void FinishBackup();
	// ������ � .reg-����
	void Write(const CString& sText);
	// ���������� ����� � .reg-�����
	void SaveKey( HKEY hRoot, LPCTSTR szKeyName, LPCTSTR szValueName = nullptr );
	// ���������� �������� � .reg-�����
	void SaveValue( LPCTSTR szName, DWORD dwType, LPBYTE pData, DWORD dwSize );

	// ���� ��� ��������� ��������� � ����� Run()
	BOOL RunAsSystem();
	// ���� ��� ��������� ���������� �������� � ����� Run()
	BOOL RunAsProcess(DWORD dwProcessID);
	// ���������� ��������� �������
	void Run();

	// Stop specified services
	void StopServices();

	// ���������� ����� (Uwe Sieber - www.uwe-sieber.de)
	bool EjectDrive(TCHAR DriveLetter);

	// ���� ������ � ������ � �������� ����������
	void EjectDrives();

	// �������� Windows Explorer
	void CloseExplorer();

	// ������ Windows Explorer
	void StartExplorer();

	// ������ ������
	void CleanFiles();

	// ������ ��������
	void CleanLogs();

	// ������ ���������
	void CleanLocalMachine();

	// ������ ������������ ������
	void CleanMountedDevices();

	// ������ Windows Search
	void CleanWindowsSearch();

	// ������ ���������������� ������
	void CleanUsers();

	// ������������ Windows
	void Reboot();

	void DoDeleteFile(LPCTSTR szPath);
	void DoDeleteLog(LPCTSTR szName);
	void DeleteKey(HKEY hRoot, const CString& sSubKey);
	void DeleteValue(HKEY hRoot, LPCTSTR szSubKey, LPCTSTR szValue);
	void CopyToClipboard(const CString& sData);

	void DoDataExchange(CDataExchange* pDX) override;	// ��������� DDX/DDV
	BOOL OnInitDialog() override;
	void OnOK() override;
	void OnCancel() override;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNMRClickReport(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCopy();
	afx_msg void OnCopyAll();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedEnable();

	DECLARE_MESSAGE_MAP()

	THREAD(CUSBOblivionDlg,RunThread)
};
