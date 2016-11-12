USB Oblivion

Copyright (C) Nikolay Raspopov, 2009-2016.
http://www.cherubicsoft.com/en/projects/usboblivion

========================================================================

USB Oblivion utility designed to erase all traces of USB-connected drives and CD-ROMs from the registry in Windows XP, Windows 2003, Windows Vista, Windows 7, Windows 8, Windows 10 32/64-bit versions. The utility has a test mode of operation, i.e. without actually removing data from the registry, and, just in case, creates a .reg-file to undo any changes. There is also a fully automatic mode. 

========================================================================

Usage:

	USBOblivion[32|64].exe [command-line options]

	Command-line options:
	
		Do real clean (simulation otherwise):
			-enable
	
		Automatic run:
			-auto
	
		Don't save a registry backup file (default: save it):
			-nosave

		Save a registry backup to this file (default: save it to My Documents):
			-save:filename

		Save working log to this file:
			-log:filename
	
		Don't create a System Restore Point:
			-norestorepoint
	
		Use language XX (hex-code):
			-lang:XX
	
		Hidden mode (if possible):
			-silent
	
		Show this help:
			-?

========================================================================

Languages:

	-lang:07 - German (by Kristine Baumgart)
	-lang:08 - Greek (by Geogeo)
	-lang:09 - English (by Nikolay Raspopov)
	-lang:0a - Spanish (by CesarRG)
	-lang:0c - French (by Mathieu Bergounioux)
	-lang:10 - Italian (by Marcello)
	-lang:12 - Korean (by 4Li)
	-lang:15 - Polish (by dmocha)
	-lang:16 - Brazilian Portuguese (by Paulo Guzmán)
	-lang:19 - Russian (by Nikolay Raspopov)
	-lang:1d - Swedish (by Ake Engelbrektson)

========================================================================

System Requirements:

  * Windows XP/2003/Vista/2008/7/8/8.1/10 32/64-bit
  * About 2 Mb of disk space. No installation needed.
  * Administrative privileges.

========================================================================

Changes:

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

========================================================================

More cleanup methods:

	USN Journal:

		Some traces can be left inside NTFS file system itself in form of "USN Journal" (Update Sequence Number Journal), or "Change Journal". It is possible to delete this journal by using standard Microsoft utility FSUTIL.EXE.

		Caution:
			Deleting the journal impacts the Indexing Service, File Replication Service (FRS), Remote Installation Service (RIS), and Remote Storage, because it would require these services to perform a complete (and time-consuming) scan of the volume. This in turn negatively impacts FRS SYSVOL replication and replication between DFS link alternates while the volume is being rescanned.
			Deleting or disabling an active journal is very time consuming, because the system must access all the records in the master file table (MFT) and set the last USN attribute to zero. This process can take several minutes, and can continue after the system restarts, if necessary. During this process, the change journal is not considered active, nor is it disabled. While the system is disabling the journal, it cannot be accessed, and all journal operations return errors. You should use extreme care when disabling an active journal, because it adversely affects other applications using the journal.

		For example to clear disk "C:" journal you can use next command line:

			fsutil usn deletejournal C:

		More info here: https://technet.microsoft.com/en-us/library/cc788042(v=ws.11).aspx

========================================================================

License:

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
