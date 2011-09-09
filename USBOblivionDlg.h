//
// USBOblivionDlg.h
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

#pragma once

#include "CtrlsResize.h"


typedef enum CKeyType
{
	mControlSet,	// ����� ������   � HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*
	mHKLM_Key,		// ����� ������   � HKEY_LOCAL_MACHINE
	mHKLM_Val,		// ����� �������� � HKEY_LOCAL_MACHINE
	mHKCU_Key,		// ����� ������   � HKEY_USERS\*
	mHKCU_Val,		// ����� �������� � HKEY_USERS\*
	mHKCR_Key,		// ����� ������   � HKEY_CLASSES_ROOT
	mHKCR_Val		// ����� �������� � HKEY_CLASSES_ROOT
} CKeyType;

typedef struct CKeyDef
{
	CKeyType	nMode;				// ��� �����
	LPCTSTR		szKeyName;			// ������� ������ ����� (NULL - �����)
	LPCTSTR		szKeySubstring;		// ��� ������ ������ ����������� � ����� �����/�������� (NULL - �����)
	LPCTSTR		szValueName;		// �������� ���������� ������� ����� (NULL - �� ���������)
	LPCTSTR		szValueSubstring;	// ��� ������ ������ ����������� � �������� ����� (NULL - �����)
	BOOL		bDeleteEmpty;		// ������� ��� ���� ���� �� ������
} CKeyDef;

template < class T1, class T2 >
class C2
{
public:
	C2() {}
	C2(const T1& f, const T2& s) : first( f ), second( s ) {}
	T1 first;
	T2 second;
};

typedef C2 < CString, CString > CStringC2;

typedef CList < CStringC2 > CStringC2List;


class CUSBOblivionDlg : public CDialog
{
public:
	CUSBOblivionDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_USBOBLIVION_DIALOG };

	BOOL		m_bEnable;		// -enable		- ������� ����� �������, ����� ���������
	BOOL		m_bAuto;		// -auto		- �������������� ������
	BOOL		m_bSave;		// -nosave		- ������ ���������� .reg-����� (��������)
	BOOL		m_bElevation;	// -elevation	- ����� � ����������� �������

protected:
	CImageList	m_oImages;		// ������ ������
	HICON		m_hIcon;		// ������ ����
	CListCtrl	m_pReport;		// �����
	int			m_nSelected;	// ������� ��������� ������ � ������
	CFile		m_oFile;		// ���� �� .reg-�����
	CCtrlResize	m_CtrlsResize;	// ��������� �������� ����������
	CRect		m_InitialRect;	// ��������� ������� ���� - ����������� �������
	DWORD		m_nDrives;		// ������� �����

	LSTATUS (WINAPI *m_pRegDeleteKeyExW) (HKEY, LPCWSTR, REGSAM, DWORD);

	enum { Information = 0, Warning, Error, Search, Done, Clean, Regedit, Lock, Eject };

	void Log(LPCTSTR szText, UINT nType = Information);
	void Log(UINT nID, UINT nType = Information);
	void Write(LPCTSTR szText);

	LPCTSTR GetKeyName(HKEY hRoot) const;
	LSTATUS RegOpenKeyFull(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired, PHKEY phkResult);
	LSTATUS RegDeleteKeyFull(HKEY hKey, const CString& sSubKey);
	LSTATUS RegDeleteValueFull(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue);

	void ProcessKey(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringList& oKeys);
	void ProcessValue(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringC2List& oValues);

	// �������� .reg-�����
	bool PrepareBackup();
	// ���������� ��������� �������
	void Run();
	// �������� .reg-�����
	void FinishBackup();

	// ���� ��� ��������� ��������� � ����� Run()
	BOOL RunAsSystem();
	// ���� ��� ��������� ���������� �������� � ����� Run()
	BOOL RunAsProcess(DWORD dwProcessID);

	// ���������� ����� (Uwe Sieber - www.uwe-sieber.de)
	bool EjectDrive(TCHAR DriveLetter);

	void DeleteFile(LPCTSTR szPath);
	void DeleteLog(LPCTSTR szName);
	void DeleteKey(HKEY hRoot, const CString& sSubKey);
	void DeleteValue(HKEY hRoot, LPCTSTR szSubKey, LPCTSTR szValue);
	void SaveKey(HKEY hRoot, LPCTSTR szKeyName, LPCTSTR szValueName = NULL);
	void SaveValue(LPCTSTR szName, DWORD dwType, LPBYTE pData, DWORD dwSize);
	void CopyToClipboard(const CString& sData);

	virtual void DoDataExchange(CDataExchange* pDX);	// ��������� DDX/DDV
	virtual BOOL OnInitDialog();
	virtual void OnOK();
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

	DECLARE_MESSAGE_MAP()
};
