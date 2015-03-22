USB Oblivion

========================================================================

USBOblivion utility designed to erase all traces of USB-connected drives and CD-ROMs from the registry in Windows XP, Windows 2003, Windows Vista, Windows 7, Windows 8 32/64-bit versions. The utility has a test mode of operation, i.e. without actually removing data from the registry, and, just in case, creates a .reg-file to undo any changes. There is also a fully automatic mode. 

Утилита USBOblivion предназначена для стирания следов подключения USB-дисков и CD-ROM'ов из реестра в Windows XP, Windows 2003, Windows Vista, Windows 7, Windows 8 как 32, так и 64-битных версиях. Утилита имеет тестовый режим работы, т.е. без фактического удаления данных из реестра, и, на всякий случай, создаёт .reg-файл для отмены всех изменений. Также имеется полностью автоматический режим работы. 

========================================================================

Инструкция:

1. Не забудьте отключить все USB-диски и CD-ROM'ы.
2. Работа утилиты требует наличия административных полномочий у текущего пользователя.
3. По-умолчанию утилита НЕ будет ничего стирать, чтобы это сделать нужно отметить соответствующую галочку.
4. По-умолчанию утилита ведёт подробный отчёт и создаст .reg-файл в папке "Документы" для отката изменений.

========================================================================

Usage:

	USBOblivion[32|64].exe [params]

Params:

	-enable - Do real clean (simulation otherwise);
	-auto - Automatic run;
	-nosave - Don't save backup .reg-file;
	-lang:XX - Use language XX (hex-code);
	-silent - Hidden mode (if possible);
	-? - Show this help.
	
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

  * Windows XP/2003/Vista/2008/7/8/8.1 32/64-bit
  * About 2 Mb of disk space. No installation needed.
  * Administrative privileges.

========================================================================

Versions:

1.10.3.0

  * Added German translation (by Kristine Baumgart)

1.10.2.0

  * Added Polish translation (by dmocha)
  * Dropped Windows 2000 support

1.10.1.0

  * Added Greek translation (by Geogeo)

1.10.0.0

  * Added key "CurrentControlSet\Control\DeviceContainers\{40258d5b-c399-5c39-b26f-a3250b527c3c}" (Windows 8)
  * Added key "CurrentControlSet\Control\DeviceClasses\{7f108a28-9833-4b3b-b780-2c6b5fa5c062}" (Windows 8)
  * Updated translations

1.9.0.0

  * Added "-silent" command-line option
  * Added key "CurrentControlSet\Control\DeviceContainers\{beb6f4cc-ba87-5134-a5c9-a2b619ef4e3a}" (Windows 8)
  * Added key "CurrentControlSet\Control\DeviceClasses\{7fccc86c-228a-40ad-8a58-f590af7bfdce}" (Windows 8)

1.8.0.0

  * Fixed GUI freezes
  * Added Italian translation

1.7.0.0

  * Added English interface
  * Added Spanish translation
  * Added Russian translation

1.6.0.0

  * Added "CurrentControlSet\Services\USBSTOR\Enum" key clean-up
  * Added "CurrentControlSet\Control\usbflags" key clean-up
  * Added "SOFTWARE\Classes\Local Settings\Software\Microsoft\Windows\Shell\MuiCache" key clean-up
  * Added "SOFTWARE\Classes\Local Settings\Software\Microsoft\Windows\CurrentVersion\SyncMgr\HandlerInstances" key clean-up
  * Project moved to Google Code: http://code.google.com/p/usboblivion/

1.5.0.0

  * Added "INFCACHE.1" file removing
  * Added "{10497b1b-ba51-44e5-8318-a65c837b6661}" key clean-up (Windows 7)
  * Added SafelyRemove key clean-up

========================================================================

Translation:

To help in USBOblivion translation you'll need to perform next steps:

  1. Download and install poEdit utility ( http://sourceforge.net/projects/poedit/ );
  2. Download POT-file ( http://usboblivion.googlecode.com/svn/trunk/USBOblivion.pot );
  3. Translate it by poEdit to your language;
  4. Save to file "USBOblivion32.exe.XX.po" or to "USBOblivion64.exe.XX.po" near USBOblivion32.exe or USBOblivion64.exe correspondingly. Where "XX" is a 2- or 4-digit hexadecimal primary language identifier for example "09" - English, "19" - Russian, "07" - German, etc.
  5. Test it. Application loads translation according to current user language. To select other language than current specify command-line option "-lang:XX" (XX - hexadecimal language code).
  6. Send PO-file to me: raspopov@cherubicsoft.com 

========================================================================

License:

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

========================================================================

Copyrights:

USB Oblivion
Copyright (C) Nikolay Raspopov, 2009-2015.
E-Mail: raspopov@cherubicsoft.com
Web site: http://code.google.com/p/usboblivion/
