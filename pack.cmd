@echo off
setlocal

set zip="%ProgramFiles%\WinRar\winrar.exe"
if exist %zip% goto zip
set zip="%ProgramFiles(x86)%\WinRar\winrar.exe"
if exist %zip% goto zip
set zip="%ProgramW6432%\WinRar\winrar.exe"
if exist %zip% goto zip
echo The WinRAR utility is missing. Please go to http://www.rarlab.com/download.htm and install WinRAR.
pause
exit /b 1
:zip

for /F "tokens=3" %%i in ( 'findstr ProductVersion res\USBOblivion.rc2' ) do set version=%%~i
if not "%version%" == "" goto version
echo Failed to calculate version.
pause
exit /b 1
:version

md "..\redist" 2>nul:
del "..\redist\usboblivion-%version%.zip" 2>nul:
%zip% a -m5 -ed -ep  -r -k -s -tl -afzip -cfg- "..\redist\usboblivion-%version%.zip" ReadMe.txt Win32\release\USBOblivion32.exe x64\release\USBOblivion64.exe
%zip% a -m5 -ed -ep1 -r -k -s -tl -afzip -cfg- "..\redist\usboblivion-%version%_src.zip" *.* -x*.db -x*.opensdf -x*.user -x*.aps -x*.sdf -x*.ncb -x*.suo -x*.zip -x*.po_ -x*\.vs\* -x*\.svn\* -x*\Win32\* -x*\x64\* -x*\ipch\*