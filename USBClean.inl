//
// USBClean.inl
//
// Copyright (c) Nikolay Raspopov, 2009-2018.
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

// Used in CleanLocalMachine() and CleanUsers() functions

static const CKeyDef defs[] =
{
	// Windows 10 Upgrades
	{ mControlSet_Key, _T( "Control\\DeviceMigration" ), NULL, NULL, NULL, TRUE },
	{ mHKLM_Key, _T( "SYSTEM\\Setup\\Upgrade" ), NULL, NULL, NULL, TRUE },
	{ mHKLM_Key, _T( "SYSTEM\\Setup\\SetupapiLogStatus" ), NULL, NULL, NULL, TRUE },
	// "USB Mass Storage Device" 7/8/10, "Universal Serial Bus controllers" XP/Vista
	{ mControlSet_Key, _T( "Control\\Class\\{36FC9E60-C465-11CF-8056-444553540000}" ), NULL, _T( "InfSection" ), _T( "USBSTOR_BULK" ), FALSE },
	// "DVD/CD-ROM drives" XP, Vista
	//{ mControlSet_Key, _T("Control\\Class\\{4D36E965-E325-11CE-BFC1-08002BE10318}"), NULL, _T("InfSection"), _T("cdrom_install"), FALSE },
	// "Disk drives" XP, Vista
	//{ mControlSet_Key, _T("Control\\Class\\{4D36E967-E325-11CE-BFC1-08002BE10318}"), NULL, _T("InfSection"), _T("disk_install"), FALSE },
	// "Portable Devices" Vista
	{ mControlSet_Key, _T( "Control\\Class\\{EEC5AD98-8080-425F-922A-DABF3DE3F69A}" ), NULL, _T( "InfSection" ), _T( "Basic_Install" ), FALSE },
	// XP, Vista
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{53f56307-b6bf-11d0-94f2-00a0c91efb8b}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{53f56308-b6bf-11d0-94f2-00a0c91efb8b}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// XP
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{53f5630a-b6bf-11d0-94f2-00a0c91efb8b}" ), _T( "STORAGE#RemovableMedia" ), NULL, NULL, FALSE },
	// XP, Vista
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}" ), _T( "STORAGE#RemovableMedia" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// Vista
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{6ac27878-a6fa-4155-ba85-f98f491d4f33}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{6ac27878-a6fa-4155-ba85-f98f491d4f33}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// Vista
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{f33fdc04-d1ac-4e8e-9a30-19bbd4b108ae}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{f33fdc04-d1ac-4e8e-9a30-19bbd4b108ae}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// Windows 7
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{10497b1b-ba51-44e5-8318-a65c837b6661}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{10497b1b-ba51-44e5-8318-a65c837b6661}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// windows 8
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{7fccc86c-228a-40ad-8a58-f590af7bfdce}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{7fccc86c-228a-40ad-8a58-f590af7bfdce}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{7f108a28-9833-4b3b-b780-2c6b5fa5c062}" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Control\\DeviceClasses\\{7f108a28-9833-4b3b-b780-2c6b5fa5c062}" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// XP
	{ mControlSet_Key, _T( "Enum\\STORAGE\\RemovableMedia" ), NULL, NULL, NULL, FALSE },
	// Vista
	{ mControlSet_Key, _T( "Enum\\STORAGE\\Volume" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Enum\\STORAGE\\Volume" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// Windows 10
	{ mControlSet_Key, _T( "Enum\\SWD\\WPDBUSENUM" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Enum\\SWD\\WPDBUSENUM" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// XP, Vista
	{ mControlSet_Key, _T( "Enum\\USBSTOR" ), NULL, NULL, NULL, TRUE },
	{ mControlSet_Key, _T( "Services\\USBSTOR\\Enum" ), NULL, NULL, NULL, TRUE },
	// Vista
	{ mControlSet_Key, _T( "Enum\\WpdBusEnumRoot\\UMB" ), _T( "USBSTOR#Disk" ), NULL, NULL, FALSE },
	{ mControlSet_Key, _T( "Enum\\WpdBusEnumRoot\\UMB" ), _T( "USBSTOR#CdRom" ), NULL, NULL, FALSE },
	// Vista
	{ mControlSet_Key, _T( "Hardware Profiles\\0000\\System\\CurrentControlSet\\Enum\\USBSTOR" ), NULL, NULL, NULL, TRUE },
	{ mControlSet_Key, _T( "Hardware Profiles\\0001\\System\\CurrentControlSet\\Enum\\USBSTOR" ), NULL, NULL, NULL, TRUE },
	{ mControlSet_Key, _T( "Hardware Profiles\\0002\\System\\CurrentControlSet\\Enum\\USBSTOR" ), NULL, NULL, NULL, TRUE },
	// Vista
	{ mHKLM_Val, _T( "SOFTWARE\\Microsoft\\WBEM\\WDM" ), _T( "USBSTOR" ), NULL, NULL, FALSE },
	// Vista
	{ mHKLM_Val, _T( "SOFTWARE\\Microsoft\\WBEM\\WDM\\DREDGE" ), _T( "USBSTOR" ), NULL, NULL, FALSE },
	// Vista ReadyBoost
	{ mHKLM_Key, _T( "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\EMDMgmt" ), NULL, NULL, NULL, FALSE },
	// Vista
	{ mHKLM_Key, _T( "SOFTWARE\\Microsoft\\Windows Portable Devices\\Devices" ), NULL, NULL, NULL, FALSE },
	// AutoPlay
	{ mHKCU_Key, _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AutoplayHandlers\\KnownDevices" ), NULL, NULL, NULL, TRUE },
	// SafelyRemove
	{ mHKCU_Key, _T( "SOFTWARE\\Software\\SafelyRemove\\Drives" ), NULL, NULL, NULL, TRUE },
	// MuiCache
	{ mHKCU_Key, _T( "SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache" ), NULL, NULL, NULL, TRUE },
	// HandlerInstances
	{ mHKCU_Key, _T( "SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\SyncMgr\\HandlerInstances" ), NULL, NULL, NULL, TRUE },
	// end
	{ mControlSet_Key, NULL, NULL, NULL, NULL, FALSE }
};

// Event logs to clear
static const LPCTSTR szLogs[] =
{
	_T( "Microsoft-Windows-DeviceSetupManager/Operational" ),
	_T( "Microsoft-Windows-DeviceSetupManager/Admin" ),
	_T( "Microsoft-Windows-Kernel-PnP/Configuration" ),
	_T( "Microsoft-Windows-Kernel-ShimEngine/Operational" ),
	_T( "Microsoft-Windows-DriverFrameworks-UserMode/Operational" ),
	_T( "HardwareEvents" ),
	_T( "Application" ),
	_T( "Security" ),
	_T( "System" )
};

// Files to delete
static const LPCTSTR szFiles[] =
{
	_T( "%SystemRoot%\\setup*.log" ),
	_T( "%SystemRoot%\\inf\\setupapi*.log" ),
	_T( "%SystemRoot%\\inf\\setupapi.ev1" ),
	_T( "%SystemRoot%\\inf\\setupapi.ev2" ),
	_T( "%SystemRoot%\\inf\\setupapi.ev3" ),
	_T( "%SystemRoot%\\inf\\INFCACHE.1" ),
	_T( "%SystemRoot%\\System32\\wbem\\Logs\\wmiprov.log" )
};

void CUSBOblivionDlg::EjectDrives()
{
	DWORD nDrives = GetLogicalDrives();
	CString drives;
	DWORD nMask = 1;
	for ( TCHAR i = _T( 'A' ); i <= _T( 'Z' ); ++i, nMask <<= 1 )
	{
		if ( ( nDrives & nMask ) != 0 )
		{
			CString drive;
			drive += i;
			drive += _T( ":" );

			UINT nType = GetDriveType( drive );
			if ( ( nType == DRIVE_REMOVABLE /*|| nType == DRIVE_CDROM*/ ) && EjectDrive( i ) )
			{
				// Извлечено!
			}
			else
			{
				// Не нужно извлекать или не удалось извлечь
				drives += drive + _T( " " );
			}
		}
	}

	// Обновление данных о дисках, чтобы дважды не сообщать
	Sleep( 2000 );
	m_nDrives = GetLogicalDrives();

	Log( LoadString( IDS_RUN_DISK_FOUND ) + drives );
}

void CUSBOblivionDlg::CloseExplorer()
{
	if ( ! m_bCloseExplorer )
		return;

	Log( IDS_RUN_EXPLORER );

	// New Restart Manager way
	if ( theApp.dyn_RmStartSession )
	{
		static const WCHAR szExplorer[] = L"\\explorer.exe";
		WCHAR szSession[ CCH_RM_SESSION_KEY + 1 ] = {};
		DWORD hSession;
		DWORD ret = theApp.dyn_RmStartSession( &hSession, 0, szSession );
		if ( ret == ERROR_SUCCESS )
		{
			bool bSuccess = false;

			DWORD idProcesses[ 1024 ] = {};
			DWORD cbNeeded = 0;
			if ( theApp.dyn_EnumProcesses( idProcesses, sizeof( idProcesses ), &cbNeeded ) )
			{
				const DWORD cProcesses = cbNeeded / sizeof( DWORD );
				for ( DWORD i = 0; i < cProcesses; ++i )
				{
					if ( idProcesses[ i ] )
					{
						if ( HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION, FALSE, idProcesses[ i ] ) )
						{
							WCHAR szProcessName[ MAX_PATH ] = {};
							RM_UNIQUE_PROCESS process = { idProcesses[ i ] };
							FILETIME tExitTime, tKernelTime, tUserTime;
							if ( theApp.dyn_GetProcessImageFileNameW( hProcess, szProcessName, MAX_PATH ) &&
								GetProcessTimes( hProcess, &process.ProcessStartTime, &tExitTime, &tKernelTime, &tUserTime ) )
							{
								const size_t nLength = wcslen( szProcessName );
								if ( nLength > _countof( szExplorer ) &&
									 _wcsicmp( szProcessName + nLength - ( _countof( szExplorer ) - 1 ), szExplorer ) == 0 )
								{
									ret = theApp.dyn_RmRegisterResources( hSession, 0, NULL, 1, &process, 0, NULL );
									if ( ret == ERROR_SUCCESS )
									{
										TRACE( _T( "Registering for shutdown \"%s\" (%u)\n" ), szProcessName, idProcesses[ i ] );
										bSuccess = true;
									}
								}
							}
						}
					}
				}
			}

			if ( bSuccess )
			{
				TRACE( _T( "Shutting down...\n" ) );
				ret = theApp.dyn_RmShutdown( hSession, RmForceShutdown, NULL );
				if ( ret == ERROR_SUCCESS )
				{
					TRACE( _T( "Shutdown Success\n" ) );
				}
				else
				{
					TRACE( _T( "Shutdown Error: %u\n" ), GetLastError() );
				}
			}

			theApp.dyn_RmEndSession( ret );
		}
	}

	// Old Undocumented
	if ( HWND hWndTray = ::FindWindow( _T( "Shell_TrayWnd" ), NULL ) )
	{
		::PostMessage( hWndTray, WM_USER + 0x01b4, 0, 0 );
	}

	// Very old way
	if ( HWND hWndProgMan = ::FindWindow( _T( "Progman" ), NULL ) )
	{
		::PostMessage( hWndProgMan, WM_QUIT, 0, FALSE );
	}
}

void CUSBOblivionDlg::StartExplorer()
{
	Log( IDS_RUN_START_EXPLORER );

	CString sExplorer;
	ExpandEnvironmentStrings( _T( "%WINDIR%\\explorer.exe" ), sExplorer.GetBuffer( MAX_PATH ), MAX_PATH );
	sExplorer.ReleaseBuffer();

	ShellExecute( NULL, NULL, sExplorer, NULL, NULL, SW_HIDE );
}

void CUSBOblivionDlg::CleanFiles()
{
	Log( IDS_RUN_FILES, Search );

	for ( int i = 0; i < _countof( szFiles ); ++i )
	{
		CString sFile;
		ExpandEnvironmentStrings( szFiles[ i ], sFile.GetBuffer( MAX_PATH ), MAX_PATH );
		sFile.ReleaseBuffer();

		DoDeleteFile( sFile );
	}
}

void CUSBOblivionDlg::CleanLogs()
{
	Log( IDS_RUN_LOGS, Search );

	for ( int i = 0; i < _countof( szLogs ); ++i )
	{
		DoDeleteLog( szLogs[ i ] );
	}
}

void CUSBOblivionDlg::CleanLocalMachine()
{
	Log( IDS_RUN_REGISTRY, Search );

	static const CString sSYS = _T( "SYSTEM" );

	CString msg;
	LRESULT ret;
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;
	DWORD dwType;
	BYTE pszValue[ 4096 ] = {};
	DWORD cchValue;

	// Сбор данных

	CStringList oControlSets;
	{
		HKEY hSYS = NULL;
		ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sSYS, KEY_READ, &hSYS );
		if ( ret == ERROR_SUCCESS )
		{
			for ( DWORD dwIndex = 0; ; ++dwIndex )
			{
				cchName = _countof( pszName );
				ret = SHEnumKeyEx( hSYS, dwIndex, pszName, &cchName );
				if ( ret != ERROR_SUCCESS )
					break;
				if ( _tcslen( pszName ) == 13 && StrCmpNI( pszName, _T( "ControlSet" ), 10 ) == 0 )
				{
					oControlSets.AddTail( pszName );
				}
			}
			RegCloseKey( hSYS );
		}
	}

	msg.Format( _T( "%d" ), (int)oControlSets.GetCount() );
	Log( LoadString( IDS_RUN_CONTROLSETS_FOUND ) + msg );

	CStringList oKeys;			// Keys marked for deletion
	CStringC2List oValues;		// Values marked for deletion
	CStringList oIDs;			// Vendor IDs marked for deletion

	for ( POSITION posControlSets = oControlSets.GetHeadPosition(); posControlSets; )
	{
		const CString sControlSetKey = sSYS + _T( "\\" ) + oControlSets.GetNext( posControlSets ) + _T( "\\" );

		// Detection of "USBSTOR#Disk" or "USBSTOR#CdRom" volumes inside "Enum\STORAGE\Volume"
		{
			const CString sVolumeKey = sControlSetKey + _T( "Enum\\STORAGE\\Volume" );
			HKEY hVolumeKey = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sVolumeKey, KEY_READ, &hVolumeKey );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwVolumeIndex = 0; ; ++dwVolumeIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hVolumeKey, dwVolumeIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					if ( StrStrI( (LPCTSTR)pszName, _T( "USBSTOR#Disk" ) ) || StrStrI( (LPCTSTR)pszName, _T( "USBSTOR#CdRom" ) ) )
					{
						dwType = 0;
						pszValue[ 0 ] = 0;
						pszValue[ 1 ] = 0;
						cchValue = sizeof( pszValue );
						ret = SHGetValue( hVolumeKey, pszName, _T( "Driver" ), &dwType, pszValue, &cchValue );
						if ( ret == ERROR_SUCCESS )
						{
							AddUnique( oKeys, sControlSetKey + _T( "Control\\Class\\" ) + (LPCTSTR)pszValue );
						}
					}
				}
				RegCloseKey( hVolumeKey );
			}
		}

		// Windows 8/10
		// Detection of "USBSTOR" device containers
		{
			// Enumerate keys of "Control\DeviceContainers\{CLSID}"
			const CString sDeviceContainersKey = sControlSetKey + _T( "Control\\DeviceContainers" );
			HKEY hDeviceContainersKey = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sDeviceContainersKey, KEY_READ, &hDeviceContainersKey );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwDeviceContainersIndex = 0; ; ++dwDeviceContainersIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hDeviceContainersKey, dwDeviceContainersIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;

					// Enumerate values of "Control\DeviceContainers\{CLSID}\BaseContainers\{CLSID}"
					const CString sBaseContainersKey = sDeviceContainersKey + _T( "\\" ) + pszName;
					HKEY hBaseContainersKey = NULL;
					ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sBaseContainersKey + _T( "\\BaseContainers\\" ) + pszName, KEY_READ, &hBaseContainersKey );
					if ( ret == ERROR_SUCCESS )
					{
						for ( DWORD dwBaseContainersIndex = 0; ; ++dwBaseContainersIndex )
						{
							pszName[ 0 ] = 0;
							cchName = _countof( pszName );
							dwType = 0;
							pszValue[ 0 ] = 0;
							pszValue[ 1 ] = 0;
							cchValue = sizeof( pszValue );
							ret = SHEnumValue( hBaseContainersKey, dwBaseContainersIndex, pszName, &cchName, &dwType, pszValue, &cchValue );
							if ( ret != ERROR_SUCCESS )
								break;
							pszValue[ cchValue ] = 0;
							pszValue[ cchValue + 1 ] = 0;
							if ( StrStrI( (LPCTSTR)pszName, _T( "USBSTOR" ) ) )
							{
								AddUnique( oKeys, sBaseContainersKey );
								break;
							}
						}
						RegCloseKey( hBaseContainersKey );
					}
				}
				RegCloseKey( hDeviceContainersKey );
			}
		}

		// XP, Vista
		// Удаление всех остальных ключей "Enum\\USB\\VID_xxx\\yyy" у которых значение "Service" равен "USBSTOR"
		{
			const CString sUSBKey = sControlSetKey + _T( "Enum\\USB" );
			HKEY hUSBKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sUSBKey, KEY_READ, &hUSBKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwUSBIndex = 0; ; ++dwUSBIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hUSBKeys, dwUSBIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					if ( CmpStrI( pszName, _T( "Vid_" ), 4 ) )
					{
						const CString sID = pszName;
						const CString sDevKey = sUSBKey + _T( "\\" ) + pszName;
						HKEY hDevKeys = NULL;
						ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sDevKey, KEY_READ, &hDevKeys );
						if ( ret == ERROR_SUCCESS )
						{
							CStringList oSubKeys;
							DWORD dwDelIndex = 0, dwDeleted = 0;
							for ( ; ; ++dwDelIndex )
							{
								pszName[ 0 ] = 0;
								cchName = _countof( pszName );
								ret = SHEnumKeyEx( hDevKeys, dwDelIndex, pszName, &cchName );
								if ( ret != ERROR_SUCCESS )
									break;

								dwType = 0;
								pszValue[ 0 ] = 0;
								pszValue[ 1 ] = 0;
								cchValue = sizeof( pszValue );
								ret = SHGetValue( hDevKeys, pszName, _T( "Service" ), &dwType, pszValue, &cchValue );
								if ( ret == ERROR_SUCCESS && CmpStrI( (LPCTSTR)pszValue, _T( "USBSTOR" ) ) )
								{
									dwDeleted++;

									AddUnique( oIDs, sID );

									AddUnique( oSubKeys, sDevKey + _T( "\\" ) + pszName );

									// Delete device container
									dwType = 0;
									pszValue[ 0 ] = 0;
									pszValue[ 1 ] = 0;
									cchValue = sizeof( pszValue );
									ret = SHGetValue( hDevKeys, pszName, _T( "ContainerID" ), &dwType, pszValue, &cchValue );
									if ( ret == ERROR_SUCCESS )
									{
										AddUnique( oKeys, sControlSetKey + _T( "Control\\DeviceContainers\\" ) + (LPCTSTR)pszValue );
									}

									// Delete driver
									dwType = 0;
									pszValue[ 0 ] = 0;
									pszValue[ 1 ] = 0;
									cchValue = sizeof( pszValue );
									ret = SHGetValue( hDevKeys, pszName, _T( "Driver" ), &dwType, pszValue, &cchValue );
									if ( ret == ERROR_SUCCESS )
									{
										AddUnique( oKeys, sControlSetKey + _T( "Control\\Class\\" ) + (LPCTSTR)pszValue );
									}
								}
							}
							RegCloseKey( hDevKeys );

							// Удалены все подключи?
							if ( dwDelIndex && dwDeleted == dwDelIndex )
							{
								// Удаление главного ключа
								AddUnique( oKeys, sDevKey );
								oSubKeys.RemoveAll();
							}

							for ( POSITION posSubKeys = oSubKeys.GetHeadPosition(); posSubKeys; )
								AddUnique( oKeys, oSubKeys.GetNext( posSubKeys ) );
						}
					}
				}
				RegCloseKey( hUSBKeys );
			}
		}

		// XP, Vista
		// Удаление ссылок на драйвера (параметр "Driver") у устройств из "Enum\USBSTOR"
		{
			const CString sFullKey = sControlSetKey + _T( "Enum\\USBSTOR" );
			HKEY hKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sFullKey, KEY_READ, &hKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;

					const CString sSubFullKey = sFullKey + _T( "\\" ) + pszName;
					HKEY hSubKeys = NULL;
					ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sSubFullKey, KEY_READ, &hSubKeys );
					if ( ret == ERROR_SUCCESS )
					{
						for ( DWORD dwSubIndex = 0; ; ++dwSubIndex )
						{
							pszName[ 0 ] = 0;
							cchName = _countof( pszName );
							ret = SHEnumKeyEx( hSubKeys, dwSubIndex, pszName, &cchName );
							if ( ret != ERROR_SUCCESS )
								break;

							dwType = 0;
							pszValue[ 0 ] = 0;
							pszValue[ 1 ] = 0;
							cchValue = sizeof( pszValue );
							ret = SHGetValue( hSubKeys, pszName, _T( "Driver" ),
								&dwType, pszValue, &cchValue );
							if ( ret == ERROR_SUCCESS )
							{
								AddUnique( oKeys, sControlSetKey + _T( "Control\\Class\\" ) + (LPCTSTR)pszValue );
							}
						}
						RegCloseKey( hSubKeys );
					}
				}
				RegCloseKey( hKeys );
			}
		}

		// XP, Vista
		// Удаление связанных пар "Control\\DeviceClasses\\{a5dcbf10-6530-11d2-901f-00c04fb951ed}" и "Enum\\USB"
		{
			const CString sFullKey = sControlSetKey + _T( "Control\\DeviceClasses\\{a5dcbf10-6530-11d2-901f-00c04fb951ed}" );
			HKEY hKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sFullKey, KEY_READ, &hKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					if ( StrStrI( pszName, _T( "USB#Vid" ) ) )
					{
						dwType = 0;
						pszValue[ 0 ] = 0;
						pszValue[ 1 ] = 0;
						cchValue = sizeof( pszValue );
						ret = SHGetValue( hKeys, pszName, _T( "DeviceInstance" ), &dwType, pszValue, &cchValue );
						if ( ret == ERROR_SUCCESS )
						{
							const CString sEnumKey = sControlSetKey + _T( "Enum\\" ) + (LPCTSTR)pszValue;
							dwType = 0;
							pszValue[ 0 ] = 0;
							pszValue[ 1 ] = 0;
							cchValue = sizeof( pszValue );
							ret = SHGetValue( HKEY_LOCAL_MACHINE, sEnumKey, _T( "Service" ), &dwType, pszValue, &cchValue );
							if ( ret == ERROR_SUCCESS && CmpStrI( (LPCTSTR)pszValue, _T( "USBSTOR" ) ) )
							{
								AddUnique( oKeys, sFullKey + _T( "\\" ) + pszName );
								AddUnique( oKeys, sEnumKey );
							}
						}
					}
				}
				RegCloseKey( hKeys );
			}
		}

		// Deletion of control flags
		LPCTSTR szControlFlags[] = { _T( "Control\\usbflags" ), _T( "Control\\usbstor" ) };
		for ( int i = 0; i < _countof( szControlFlags ); ++i )
		{
			const CString sFullKey = sControlSetKey + szControlFlags[ i ];
			HKEY hKeys = NULL;
			ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sFullKey, KEY_READ, &hKeys );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;

					// VID_AAAA&PID_BBBB == AAAABBBBCCCC
					const CString sName = pszName;
					const CString sUsbflagsID = _T( "vid_" ) + sName.Mid( 0, 4 ) + _T( "&pid_" ) + sName.Mid( 4, 4 );
					for ( POSITION posIDs = oIDs.GetHeadPosition(); posIDs; )
					{
						const CString sID = oKeys.GetNext( posIDs );
						if ( sID.CompareNoCase( sUsbflagsID ) == 0 )
						{
							AddUnique( oKeys, sFullKey + _T( "\\" ) + pszName );
							break;
						}
					}
				}
				RegCloseKey( hKeys );
			}
		}

		// Подчистка всего остального
		for ( int i = 0; defs[ i ].szKeyName; ++i )
		{
			if ( defs[ i ].nMode == mControlSet_Key )
				ProcessKey( HKEY_LOCAL_MACHINE, sControlSetKey, defs[ i ], oKeys );
			else if ( defs[ i ].nMode == mControlSet_Val )
				ProcessValue( HKEY_LOCAL_MACHINE, sControlSetKey, defs[ i ], oValues );
		}
	}

	// Обработка ключей
	for ( int i = 0; defs[ i ].szKeyName; ++i )
	{
		if ( defs[ i ].nMode == mHKLM_Key )
			ProcessKey( HKEY_LOCAL_MACHINE, CString(), defs[ i ], oKeys );
		else if ( defs[ i ].nMode == mHKCR_Key )
			ProcessKey( HKEY_CLASSES_ROOT, CString(), defs[ i ], oKeys );
	}

	// Обработка переменных
	for ( int i = 0; defs[ i ].szKeyName; ++i )
	{
		if ( defs[ i ].nMode == mHKLM_Val )
			ProcessValue( HKEY_LOCAL_MACHINE, CString(), defs[ i ], oValues );
		else if ( defs[ i ].nMode == mHKCR_Val )
			ProcessValue( HKEY_CLASSES_ROOT, CString(), defs[ i ], oValues );
	}

	msg.Format( _T( "%d" ), (int)oKeys.GetCount() );
	Log( LoadString( IDS_RUN_KEYS_FOUND ) + msg );
	msg.Format( _T( "%d" ), (int)oValues.GetCount() );
	Log( LoadString( IDS_RUN_VALUES_FOUND ) + msg );

	// Удаление

	if ( oKeys.GetCount() || oValues.GetCount() )
		Log( IDS_RUN_CLEAN, Clean );

	for ( POSITION posKeys = oKeys.GetHeadPosition(); posKeys; )
	{
		DeleteKey( HKEY_LOCAL_MACHINE, oKeys.GetNext( posKeys ) );
	}

	for ( POSITION posValues = oValues.GetHeadPosition(); posValues; )
	{
		const CStringC2& pValue = oValues.GetNext( posValues );
		DeleteValue( HKEY_LOCAL_MACHINE, pValue.first, pValue.second );
	}
}

void CUSBOblivionDlg::CleanMountedDevices()
{
	static const CString sMD = _T( "SYSTEM\\MountedDevices" );

	Log( IDS_RUN_MOUNTS, Search );

	DWORD nDrives = GetLogicalDrives();
	CString msg;
	LRESULT ret;
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;
	DWORD dwType;
	BYTE pszValue[ 4096 ] = {};
	DWORD cchValue;

	CStringList oMountedDevs;
	{
		HKEY hMD = NULL;
		ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sMD, KEY_READ, &hMD );
		if ( ret == ERROR_SUCCESS )
		{
			for ( DWORD dwIndex = 0; ; ++dwIndex )
			{
				pszName[ 0 ] = 0;
				cchName = _countof( pszName );
				dwType = 0;
				pszValue[ 0 ] = 0;
				pszValue[ 1 ] = 0;
				cchValue = sizeof( pszValue );
				ret = SHEnumValue( hMD, dwIndex, pszName, &cchName, &dwType, pszValue, &cchValue );
				if ( ret != ERROR_SUCCESS )
					break;
				pszValue[ cchValue ] = 0;
				pszValue[ cchValue + 1 ] = 0;
				if ( StrStrI( (LPCTSTR)pszValue, _T( "USBSTOR#Disk" ) ) ||
					 StrStrI( (LPCTSTR)pszValue, _T( "USBSTOR#CdRom" ) ) ||
					 StrStrI( (LPCTSTR)pszValue, _T( "STORAGE#RemovableMedia" ) ) )
				{
					oMountedDevs.AddTail( pszName );
				}
			}
			RegCloseKey( hMD );
		}
	}

	msg.Format( _T( "%d" ), (int)oMountedDevs.GetCount() );
	Log( LoadString( IDS_RUN_MOUNT_FOUND ) + msg );

	// Удаление

	bool bLog = true;

	for ( POSITION posMountedDevs = oMountedDevs.GetHeadPosition(); posMountedDevs; )
	{
		POSITION posCurrent = posMountedDevs;
		CString sDev = oMountedDevs.GetNext( posMountedDevs );

		// HKEY_LOCAL_MACHINE\SYSTEM\MountedDevices
		// -> "\DosDevices\X:"
		if ( sDev.GetLength() == 14 && sDev.Left( 12 ) == _T( "\\DosDevices\\" ) )
		{
			// не трогать подключенные устройства
			if ( ( nDrives & ( 1 << ( sDev.GetAt( 12 ) - _T( 'A' ) ) ) ) == 0 )
			{
				if ( bLog )
				{
					bLog = false;
					Log( IDS_RUN_CLEAN, Clean );
				}
				DeleteValue( HKEY_LOCAL_MACHINE, sMD, sDev );
				oMountedDevs.RemoveAt( posCurrent );
			}
		}
		// -> "\??\Volume{CLSID}"
		else if ( sDev.GetLength() == 48 && sDev.Left( 11 ) == _T( "\\??\\Volume{" ) )
		{
			if ( bLog )
			{
				bLog = false;
				Log( IDS_RUN_CLEAN, Clean );
			}
			DeleteValue( HKEY_LOCAL_MACHINE, sMD, sDev );
			oMountedDevs.RemoveAt( posCurrent );
		}
	}
}

void CUSBOblivionDlg::CleanWindowsSearch()
{
	// Example: "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows Search\VolumeInfoCache\E:"
	static const CString sVolumeInfoCache = _T( "SOFTWARE\\Microsoft\\Windows Search\\VolumeInfoCache" );

	Log( IDS_RUN_WINDOWS_SEARCH, Search );

	DWORD nDrives = GetLogicalDrives();
	LRESULT ret;
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;

	bool bLog = true;

	HKEY hKeys = NULL;
	ret = RegOpenKeyFull( HKEY_LOCAL_MACHINE, sVolumeInfoCache, KEY_READ, &hKeys );
	if ( ret == ERROR_SUCCESS )
	{
		for ( DWORD dwIndex = 0; ; ++dwIndex )
		{
			pszName[ 0 ] = 0;
			cchName = _countof( pszName );
			ret = SHEnumKeyEx( hKeys, dwIndex, pszName, &cchName );
			if ( ret != ERROR_SUCCESS )
				break;

			// не трогать подключенные устройства
			if ( pszName[ 1 ] == _T( ':' ) && ( nDrives & ( 1 << ( pszName[ 0 ] - _T( 'A' ) ) ) ) == 0 )
			{
				if ( bLog )
				{
					bLog = false;
					Log( IDS_RUN_CLEAN, Clean );
				}
				DeleteKey( HKEY_LOCAL_MACHINE, sVolumeInfoCache + _T( "\\" ) + pszName );
			}
		}
	}
}

void CUSBOblivionDlg::CleanUsers()
{
	static const CString sMP2 = _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2" );
	static const CString sCPC = _T( "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2\\CPC\\Volume" );

	Log( IDS_RUN_USERS, Search );

	DWORD nDrives = GetLogicalDrives();
	CString msg;
	LRESULT ret;
	TCHAR pszName[ 1024 ] = {};
	DWORD cchName;

	// Перечисление пользователей
	CStringList oUsers;
	for ( DWORD dwIndex = 0; ; ++dwIndex )
	{
		cchName = _countof( pszName );
		ret = SHEnumKeyEx( HKEY_USERS, dwIndex, pszName, &cchName );
		if ( ret != ERROR_SUCCESS )
			break;
		if ( StrStrI( pszName, _T( "_Classes" ) ) == 0 ) // исключить "_Classes"
		{
			oUsers.AddTail( pszName );
		}
	}

	msg.Format( _T( "%d" ), (int)oUsers.GetCount() );
	Log( LoadString( IDS_RUN_USERS_FOUND ) + msg );

	for ( POSITION posUsers = oUsers.GetHeadPosition(); posUsers; )
	{
		CStringList oPoints, oVolumes;
		CString sKey = oUsers.GetNext( posUsers ) + _T( "\\" );

		Log( LoadString( IDS_RUN_USER ) + sKey.Left( sKey.Find( _T( '\\' ) ) ), Search );

		CStringList oUKeys;
		CStringC2List oUValues;

		// Обработка ключей
		for ( int i = 0; defs[ i ].szKeyName; ++i )
		{
			if ( defs[ i ].nMode == mHKCU_Key )
				ProcessKey( HKEY_USERS, sKey, defs[ i ], oUKeys );
		}

		// Обработка переменных
		for ( int i = 0; defs[ i ].szKeyName; ++i )
		{
			if ( defs[ i ].nMode == mHKCU_Val )
				ProcessValue( HKEY_USERS, sKey, defs[ i ], oUValues );
		}

		{
			HKEY hMP2 = NULL;
			ret = RegOpenKeyFull( HKEY_USERS, sKey + sMP2, KEY_READ, &hMP2 );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hMP2, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					AddUnique( oPoints, sKey + sMP2 + _T( "\\" ) + pszName );
				}
				RegCloseKey( hMP2 );
			}
		}

		msg.Format( _T( "%d" ), (int)oPoints.GetCount() );
		Log( LoadString( IDS_RUN_MOUNT_FOUND ) + msg );

		{
			HKEY hCPC = NULL;
			ret = RegOpenKeyFull( HKEY_USERS, sKey + sCPC, KEY_READ, &hCPC );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hCPC, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					AddUnique( oVolumes, sKey + sCPC + _T( "\\" ) + pszName );
				}
				RegCloseKey( hCPC );
			}
		}

		msg.Format( _T( "%d" ), (int)oVolumes.GetCount() );
		Log( LoadString( IDS_RUN_EXPLORER_FOUND ) + msg );

		{
			HKEY hCPC = NULL;
			ret = RegOpenKeyFull( HKEY_USERS, sKey + sCPC, KEY_READ, &hCPC );
			if ( ret == ERROR_SUCCESS )
			{
				for ( DWORD dwIndex = 0; ; ++dwIndex )
				{
					pszName[ 0 ] = 0;
					cchName = _countof( pszName );
					ret = SHEnumKeyEx( hCPC, dwIndex, pszName, &cchName );
					if ( ret != ERROR_SUCCESS )
						break;
					AddUnique( oVolumes, sKey + sCPC + _T( "\\" ) + pszName );
				}
				RegCloseKey( hCPC );
			}
		}

		// Удаление

		bool bLog = true;

		if ( oUKeys.GetCount() || oUValues.GetCount() )
		{
			bLog = false;
			Log( IDS_RUN_CLEAN, Clean );
		}

		for ( POSITION posKeys = oUKeys.GetHeadPosition(); posKeys; )
		{
			DeleteKey( HKEY_USERS, oUKeys.GetNext( posKeys ) );
		}

		for ( POSITION posValues = oUValues.GetHeadPosition(); posValues; )
		{
			const CStringC2& pValue = oUValues.GetNext( posValues );
			DeleteValue( HKEY_USERS, pValue.first, pValue.second );
		}

		for ( POSITION posPoints = oPoints.GetHeadPosition(); posPoints; )
		{
			POSITION posCurrent = posPoints;
			CString sPoint = oPoints.GetNext( posPoints );
			CString sPointName = sPoint.Mid( sPoint.ReverseFind( _T( '\\' ) ) + 1 );
			if ( sPointName.GetLength() == 1 &&
				// не трогать подключенные устройства
				( nDrives & ( 1 << ( sPointName.GetAt( 0 ) - _T( 'A' ) ) ) ) == 0 )
			{
				if ( bLog )
				{
					bLog = false;
					Log( IDS_RUN_CLEAN, Clean );
				}
				DeleteKey( HKEY_USERS, sPoint );
				oPoints.RemoveAt( posCurrent );
			}
		}

		for ( POSITION posVolumes = oVolumes.GetHeadPosition(); posVolumes; )
		{
			POSITION posCurrentVolumes = posVolumes;
			CString sVolume = oVolumes.GetNext( posVolumes );
			CString sVolumeName = sVolume.Mid( sVolume.ReverseFind( _T( '\\' ) ) + 1 );
			if ( bLog )
			{
				bLog = false;
				Log( IDS_RUN_CLEAN, Clean );
			}
			for ( POSITION posPoints = oPoints.GetHeadPosition(); posPoints; )
			{
				POSITION posCurrentPoints = posPoints;
				CString sPoint = oPoints.GetNext( posPoints );
				CString sPointName = sPoint.Mid( sPoint.ReverseFind( _T( '\\' ) ) + 1 );

				if ( sPointName == sVolumeName )
				{
					DeleteKey( HKEY_USERS, sPoint );
					oPoints.RemoveAt( posCurrentPoints );
					break;
				}
			}
			DeleteKey( HKEY_USERS, sVolume );
			oVolumes.RemoveAt( posCurrentVolumes );
		}
	}
}

void CUSBOblivionDlg::Reboot()
{
	if ( ! m_bReboot )
		return;

	Log( IDS_RUN_REBOOT );

	CAccessToken oToken;
	if ( oToken.GetProcessToken( TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY ) )
	{
		// Для перезагрузки
		if ( !oToken.EnablePrivilege( SE_SHUTDOWN_NAME ) )
			TRACE( _T( "EnablePrivilege(SE_SHUTDOWN_NAME) error: %d\n" ), GetLastError() );
	}
	else
		TRACE( _T( "CAccessToken::GetProcessToken error: %d\n" ), GetLastError() );

	if ( ! ExitWindowsEx( EWX_REBOOT, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_RECONFIG | SHTDN_REASON_FLAG_PLANNED ) )
	{
		CString msg;
		msg.Format( _T( "%u" ), GetLastError() );
		Log( LoadString( IDS_ERROR_REBOOT ) + msg, Error );

		if ( m_bCloseExplorer )
		{
			// Восстановление Проводника
			StartExplorer();
		}
	}
}
