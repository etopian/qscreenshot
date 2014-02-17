@echo off
setlocal
set MAKE=mingw32-make -j5

@echo Creating qScreenshot Project
svn checkout http://qscreenshot.googlecode.com/svn/trunk/ qscreenshot
cd qscreenshot\qScreenshot
@echo Start time:  %TIME%>..\..\logs.txt
call qconf.cmd
configure
%MAKE% -f makefile
@echo Finish time: %TIME%>>..\..\logs.txt
move /Y src\release\qscreenshot.exe ..\..\qscreenshot.exe
lrelease src/src.pro
move /Y src\lang\qscreenshot_ru.qm ..\..\qscreenshot_ru.qm
cd ..\..
@echo Archiving qScreenshot Nightly Build
call 7z a -mx9 "qscreenshot-r155-win32.7z" "qscreenshot.exe"
@echo Completed
exit
REM pause & pause
REM @echo Uploading archived qScreenshot Nightly Build to Google Code
REM call googlecode_upload.py -p "qscreenshot" -s "qScreenshot Nightly Build || Qt 4.8.5 || For test only!" -l "NightlyBuild,Windows,Archive" "qscreenshot-r155-win32.7z"
REM @echo Completed
REM pause & pause
