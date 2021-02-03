 USB Oblivion
---------------------------------

Copyright (C) Nikolay Raspopov, 2009-2021.
http://www.cherubicsoft.com/en/projects/usboblivion

USB Oblivion utility designed to erase all traces of (broken) USB-connected drives and CD-ROMs from the registry in Windows XP, Windows 2003, Windows Vista, Windows 7, Windows 8, Windows 10 32/64-bit versions.

The utility has a test mode of operation, i.e. without actually removing data from the registry, and, just in case, creates a .reg-file to undo any changes. There is also a fully automatic mode.

 Warnings
----------------------

	* Eject all USB drives before clean
	* Required an administrative privileges
	* Required an immediate Windows restart after clean
	* No you can not clean Windows installed on USB drive

FAQ
----------------------

Q: Why Windows after clean can't detect my USB drive?

	A: Cached data about USB drive somehow (for example you ignored a Windows restart or just no luck) was written back to the registry, so drive now erroneously partially registered. You must run utility again to clean up the mess and immediately restart Window. Repeat twice.

Q: Why my USB Hewlett-Packard printer not working after clean?

	A: Some HP printers comes with "HP Smart Install" technology - when a USB CD-ROM with drivers to the printer appears in the system on printer connection. To avoid this you must:
		1) Delete the printer from the system before clean
		2) Switch off "HP Smart Install" on both the computer and the printer using the proprietary HP utility SIUtility.exe (or SIUtility64.exe) (usually in the UTILS folder of CD with printer drivers)
		3) After that printer will be re-installed as a regular USB printer and utility will not affect it anymore

	More info about "HP Smart Install" here: http://www.hp.com/hpinfo/newsroom/press_kits/2010/plugandprint/pdf/Smart_Install_FAQ.pdf

Q: How to restore a saved .reg-file?

	A1: You can also restore from a System Restore Point: https://support.microsoft.com/en-us/help/12415/windows-10-recovery-options

	A2: You must import saved .reg-file using System account, the administrative privileges are not sufficient:
		1) Download PsExec utility here: https://docs.microsoft.com/en-us/sysinternals/downloads/psexec
		2) Run cmd.exe as Administrator and type on its console window:

			psexec.exe -i -s regedit.exe

		3) In opened Registry Editor import .reg-file using menu File -> Import...
		4) Restart Windows

 Usage
----------------------

	USBOblivion[32|64].exe [command-line options]

 Command-line options:
---------

	-enable         - Do real clean (simulation otherwise)
	-auto           - Automatic run
	-nosave         - Don't save a registry backup file
	-save:filename  - Save a registry backup to this file
	-log:filename   - Save working log to this file
	-norestorepoint - Don't create a System Restore Point
	-norestart      - Don't restart Windows
	-noexplorer     - Don't close Windows Explorer
	-lang:XX        - Use language XX (hex-code)
	-silent         - Hidden mode (if possible)
	-?              - Show this help

 Languages
---------

	-lang:04 - Simplified Chinese (by 天路)
	-lang:07 - German (by Kristine Baumgart)
	-lang:08 - Greek (by Geogeo)
	-lang:09 - English (by Nikolay Raspopov)
	-lang:0a - Spanish (by CesarRG, Boris Gilmar Terrazas Miranda)
	-lang:0c - French (by Mathieu Bergounioux, Rico-sos)
	-lang:10 - Italian (by Marcello Gianola)
	-lang:12 - Korean (by 4Li)
	-lang:13 - Dutch (by Jeroen Westera)
	-lang:15 - Polish (by dmocha)
	-lang:16 - Brazilian Portuguese (by Paulo Guzmán, Newton Apostolico)
	-lang:19 - Russian (by Nikolay Raspopov)
	-lang:1d - Swedish (by Ake Engelbrektson, Sopor)
	-lang:1f - Turkish (by Murat)
	-lang:20 - Czech (by Jaaka)

 System Requirements
----------------------

  * Windows 2000/XP/2003/Vista/2008/7/8/8.1/10 32/64-bit
  * About 3 Mb of disk space, no installation needed
  * Administrative privileges

 Changes
----------------------

1.15.1.0

	* Preserving desktop settings
	* Updated German translation (by Mr. Update)

1.15.0.0

	* Added cleaning of "Bags" and "BagMRU" keys
	* Updated Swedish translation (by Sopor)

1.14.0.0

	* Added cleaning of "Control\DeviceClasses\{6ead3d82-25ec-46bc-b7fd-c1f0df8f5037}" key

1.13.0.0

	* Added "Microsoft-Windows-Partition/Diagnostic" journal cleanup

1.12.3.0

	* Added Simplified Chinese translation (by 天路)

1.12.2.0

	* Added cleaning of "Unknown USB Device" entries
	* Updated German translation (by Steven)

1.12.1.1

	* Fixed compatibility for very old CPU
	* Updated Italian translation (by Marcello Gianola)

1.12.1.0

	* Added Windows Services closing (VDS, ReadyBoost, SuperFetch)
	* Added ReadyBoost entries removing
	* Added Dutch translation (by Jeroen Westera)
	* Updated Spanish translation (by Boris Gilmar Terrazas Miranda)

1.12.0.0

	* Added cleaning of "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet\*\Control\DeviceMigration" key
	* Added cleaning of "HKEY_LOCAL_MACHINE\SYSTEM\Setup\Upgrade" key
	* Added cleaning of "HKEY_LOCAL_MACHINE\SYSTEM\Setup\SetupapiLogStatus" key
	* Added removing of "%SystemRoot%\inf\setupapi.ev*" files (* = 1,2,3)

1.11.6.0

	* Added restart Windows and close Windows Explorer options

1.11.5.0

	* Updated Greek translation (by Geogeo)
	* Updated Brazilian Portuguese translation (by Newton Apostolico)
	* Updated French translation (by Rico-sos)

1.11.4.0

	* Added Czech translation (by Jaaka)

1.11.3.0

	* Added Turkish translation (by Murat)
	* Windows 2000 compatibility
	* Updated Polish translation (by dmocha)

1.11.2.0

	* Added "Microsoft-Windows-DeviceSetupManager/Operational" journal cleanup
	* Added "Microsoft-Windows-DeviceSetupManager/Admin" journal cleanup
	* Added "Microsoft-Windows-Kernel-PnP/Configuration" journal cleanup
	* Added "Microsoft-Windows-Kernel-ShimEngine/Operational" journal cleanup
	* Added "Control\usbstor" registry key selective cleanup
	* Fixed USB-HDD cleanup
	* Minor fixes
	* Updated Italian translation (by Marcello Gianola)
	* Updated Polish translation (by dmocha)
	* Updated Korean translation (by 4Li)

1.11.1.0

	* Added "HKLM\SOFTWARE\Microsoft\Windows Search\VolumeInfoCache" registry key cleanup
	* Extended "%SystemRoot%\setup*.log" and "%SystemRoot%\inf\setupapi*.log" files deletion
	* Minor fixes
	* Updated Italian translation (by Marcello Gianola)

1.11.0.0 (11 Oct 2016)

	* Added Windows 10 support
	* Added "Microsoft-Windows-DriverFrameworks-UserMode/Operational" journal cleanup
	* Added "Enum\SWD\WPDBUSENUM" registry key cleanup
	* Added file deletion on reboot
	* Added creation of System Restore Point (and "-norestorepoint" command-line option)
	* Added "-log" command-line option
	* Added "-save" command-line option
	* Added VS2015 compilation
	* Fixed registry backup
	* Fixed "DeviceContainers" registry key cleanup
	* Fixed too aggressive cleaning of "usbflags" registry key
	* Translations must be updated

1.10.3.0 (13 Apr 2015)

	* Added German translation (by Kristine Baumgart)
	* Project moved from closed Google Code to SourceForge.Net
	* Added VS2013 compilation

 More cleanup methods
----------------------

 USN Journal
---------

	Some traces can be left inside NTFS file system itself in form of "USN Journal" (Update Sequence Number Journal), or "Change Journal". It is possible to delete this journal by using standard Microsoft utility FSUTIL.EXE.

	Caution:
		Deleting the journal impacts the Indexing Service, File Replication Service (FRS), Remote Installation Service (RIS), and Remote Storage, because it would require these services to perform a complete (and time-consuming) scan of the volume. This in turn negatively impacts FRS SYSVOL replication and replication between DFS link alternates while the volume is being rescanned.
		Deleting or disabling an active journal is very time consuming, because the system must access all the records in the master file table (MFT) and set the last USN attribute to zero. This process can take several minutes, and can continue after the system restarts, if necessary. During this process, the change journal is not considered active, nor is it disabled. While the system is disabling the journal, it cannot be accessed, and all journal operations return errors. You should use extreme care when disabling an active journal, because it adversely affects other applications using the journal.

	For example to clear disk "C:" journal you can use next command line:

		fsutil usn deletejournal C:

	More info here: https://technet.microsoft.com/en-us/library/cc788042(v=ws.11).aspx

 License
----------------------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
