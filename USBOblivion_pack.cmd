set version=1.10.2.0
md "..\redist"
del "..\redist\usboblivion32-%version%.zip"
del "..\redist\usboblivion64-%version%.zip"
"%ProgramFiles%\WinRar\winrar.exe" a -ep -m5 -cfg- -afzip "..\redist\usboblivion32-%version%.zip" ReadMe.txt Win32\release\USBOblivion32.exe
"%ProgramFiles%\WinRar\winrar.exe" a -ep -m5 -cfg- -afzip "..\redist\usboblivion64-%version%.zip" ReadMe.txt x64\release\USBOblivion64.exe
