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
	mControlSet_Key,// Поиск ключей   в HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*
	mControlSet_Val,// Поиск значений в HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*
	mHKLM_Key,		// Поиск ключей   в HKEY_LOCAL_MACHINE
	mHKLM_Val,		// Поиск значений в HKEY_LOCAL_MACHINE
	mHKCU_Key,		// Поиск ключей   в HKEY_USERS\*
	mHKCU_Val,		// Поиск значений в HKEY_USERS\*
	mHKCR_Key,		// Поиск ключей   в HKEY_CLASSES_ROOT
	mHKCR_Val		// Поиск значений в HKEY_CLASSES_ROOT
};

using CKeyDef = struct
{
	CKeyType	nMode;				// Тип ключа
	LPCTSTR		szKeyName;			// Открыть данную папку (NULL - конец)
	LPCTSTR		szKeySubstring;		// Эта строка должна содержаться в имени ключа/значения (NULL - любая)
	LPCTSTR		szValueName;		// Проверка присутствия данного ключа (NULL - не проверять)
	LPCTSTR		szValueSubstring;	// Эта строка должна содержаться в значении ключа (NULL - любое)
	BOOL		bDeleteEmpty;		// Удалить сам ключ если он пустой
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
	CImageList	m_oImages;		// Иконки списка
	HICON		m_hIcon;		// Иконка окна
	CListCtrl	m_pReport;		// Отчёт
	int			m_nSelected;	// Текущая выбранная строка в отчёте
	CStdioFile	m_oFile;		// Путь до .reg-файла
	CCtrlResize	m_CtrlsResize;	// Изменение размеров интерфейса
	CRect		m_InitialRect;	// Начальные размеры окна - минимальные размеры
	DWORD		m_nDrives;		// Текущие диски

	CString		m_sDeleteKeyString;		// Кэш строки
	CString		m_sDeleteValueString;	// Кэш строки

	using CLogItem = C2< CString, UINT >;
	using CLogList = CList< CLogItem* >;
	cs						m_pSection;		// Синхронизация доступа к отчёту
	CAutoPtr< CLogList >	m_pReportList;	// Список строк для отчёта
	bool					m_bRunning;		// Флаг обработки

	enum { Information = 0, Warning, Error, Search, Done, Clean, Regedit, Lock, Eject };

	void Log(const CString& sText, UINT nType = Information);
	void Log(UINT nID, UINT nType = Information);

	static CString GetKeyName(HKEY hRoot);
	LSTATUS RegOpenKeyFull(HKEY hKey, LPCTSTR lpSubKey, REGSAM samDesired, PHKEY phkResult);
	LSTATUS RegDeleteKeyFull(HKEY hKey, const CString& sSubKey);
	LSTATUS RegDeleteValueFull(HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValue);

	void ProcessKey(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringList& oKeys);
	void ProcessValue(HKEY hRoot, const CString& sRoot, const CKeyDef& def, CStringC2List& oValues);

	// Открытие .reg-файла
	bool PrepareBackup();
	// Закрытие .reg-файла
	void FinishBackup();
	// Запись в .reg-файл
	void Write(const CString& sText);
	// Сохранение ключа в .reg-файле
	void SaveKey( HKEY hRoot, LPCTSTR szKeyName, LPCTSTR szValueName = nullptr );
	// Сохранение значения в .reg-файле
	void SaveValue( LPCTSTR szName, DWORD dwType, LPBYTE pData, DWORD dwSize );

	// Вход под системный аккаунтом и вызов Run()
	BOOL RunAsSystem();
	// Вход под аккаунтом указанного процесса и вызов Run()
	BOOL RunAsProcess(DWORD dwProcessID);
	// Собственно обработка реестра
	void Run();

	// Stop specified services
	void StopServices();

	// Извлечение диска (Uwe Sieber - www.uwe-sieber.de)
	bool EjectDrive(TCHAR DriveLetter);

	// Сбор данных о дисках с попыткой извлечения
	void EjectDrives();

	// Закрытие Windows Explorer
	void CloseExplorer();

	// Запуск Windows Explorer
	void StartExplorer();

	// Чистка файлов
	void CleanFiles();

	// Чистка журналов
	void CleanLogs();

	// Чистка драйверов
	void CleanLocalMachine();

	// Чистка подключённых дисков
	void CleanMountedDevices();

	// Чистка Windows Search
	void CleanWindowsSearch();

	// Чистка пользовательских данных
	void CleanUsers();

	// Перезагрузка Windows
	void Reboot();

	void DoDeleteFile(LPCTSTR szPath);
	void DoDeleteLog(LPCTSTR szName);
	void DeleteKey(HKEY hRoot, const CString& sSubKey);
	void DeleteValue(HKEY hRoot, LPCTSTR szSubKey, LPCTSTR szValue);
	void CopyToClipboard(const CString& sData);

	void DoDataExchange(CDataExchange* pDX) override;	// поддержка DDX/DDV
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
