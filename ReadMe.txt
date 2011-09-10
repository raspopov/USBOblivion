USB Oblivion

========================================================================

Утилита USBOblivion предназначена для стирания следов подключения USB-дисков и
CD-ROM'ов из реестра в Windows 2000, Windows XP, Windows 2003, Windows Vista,
Windows 7 как 32, так и 64-битных версиях. Утилита имеет тестовый режим работы,
т.е. без фактического удаления данных из реестра, и, на всякий случай,
создаёт .reg-файл для отмены всех изменений. Также имеется полностью
автоматический режим работы. 

Инструкция:

1. Не забудьте отключить все USB-диски и CD-ROM'ы.
2. Работа утилиты требует наличия административных полномочий у текущего пользователя.
3. По-умолчанию утилита НЕ будет ничего стирать, чтобы это сделать нужно отметить соответствующую галочку.
4. По-умолчанию утилита ведёт подробный отчёт и создаст .reg-файл в папке "Документы" для отката изменений.

Утилита поддерживает автоматический режим работы, параметры командной строки:

	-enable	- Рабочий режим очистки, иначе симуляция;
	-auto	- Автоматический запуск (не влияет на режим очистки);
	-nosave	- Отмена сохранения .reg-файла;
	-?	- Выдача справки.

========================================================================

System Requirements:

  * Windows 2000/XP/2003/Vista/2008/7 32/64-bit
  * About 500 Kb of disk space. No installation needed.

========================================================================

Versions:

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

Copyrights:

USB Oblivion
Copyright (C) Nikolay Raspopov, 2009-2011.
E-Mail: ryo-oh-ki@narod.ru
Web site: http://www.cherubicsoft.com/projects/usboblivion
