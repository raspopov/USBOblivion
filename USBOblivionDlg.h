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
	mControlSet,	// Поиск ключей   в HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*
	mHKLM_Key,		// Поиск ключей   в HKEY_LOCAL_MACHINE
	mHKLM_Val,		// Поиск значений в HKEY_LOCAL_MACHINE
	mHKCU_Key,		// Поиск ключей   в HKEY_USERS\*
	mHKCU_Val,		// Поиск значений в HKEY_USERS\*
	mHKCR_Key,		// Поиск ключей   в HKEY_CLASSES_ROOT
	mHKCR_Val		// Поиск значений в HKEY_CLASSES_ROOT
} CKeyType;

typedef struct CKeyDef
{
	CKeyType	nMode;				// Тип ключа
	LPCTSTR		szKeyName;			// Открыть данную папку (NULL - конец)
	LPCTSTR		szKeySubstring;		// Эта строка должна содержаться в имени ключа/значения (NULL - любая)
	LPCTSTR		szValueName;		// Проверка присутсвия данного ключа (NULL - не проверять)
	LPCTSTR		szValueSubstring;	// Эта строка должна содержаться в значении ключа (NULL - любое)
	BOOL		bDeleteEmpty;		// Удалить сам ключ если он пустой
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

	BOOL		m_bEnable;		// -enable		- Рабочий режим очистки, иначе симуляция
	BOOL		m_bAuto;		// -auto		- Автоматический запуск
	BOOL		m_bSave;		// -nosave		- Отмена сохранения .reg-файла (инверсия)
	BOOL		m_bElevation;	// -elevation	- Режим с повышенными правами

protected:
	CImageList	m_oImages;		// Иконки списка
	HICON		m_hIcon;		// Иконка окна
	CListCtrl	m_pReport;		// Отчёт
	int			m_nSelected;	// Текущая выбранная строка в отчёте
	CFile		m_oFile;		// Путь до .reg-файла
	CCtrlResize	m_CtrlsResize;	// Изменение размеров интерфейса
	CRect		m_InitialRect;	// Начальные разсеры окна - минимальные размеры
	DWORD		m_nDrives;		// Текущие диски

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

	// Открытие .reg-файла
	bool PrepareBackup();
	// Собственно обработка реестра
	void Run();
	// Закрытие .reg-файла
	void FinishBackup();

	// Вход под системный аккаунтом и вызов Run()
	BOOL RunAsSystem();
	// Вход под аккаунтом указанного процесса и вызов Run()
	BOOL RunAsProcess(DWORD dwProcessID);

	// Извлечение диска (Uwe Sieber - www.uwe-sieber.de)
	bool EjectDrive(TCHAR DriveLetter);

	void DeleteFile(LPCTSTR szPath);
	void DeleteLog(LPCTSTR szName);
	void DeleteKey(HKEY hRoot, const CString& sSubKey);
	void DeleteValue(HKEY hRoot, LPCTSTR szSubKey, LPCTSTR szValue);
	void SaveKey(HKEY hRoot, LPCTSTR szKeyName, LPCTSTR szValueName = NULL);
	void SaveValue(LPCTSTR szName, DWORD dwType, LPBYTE pData, DWORD dwSize);
	void CopyToClipboard(const CString& sData);

	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV
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
