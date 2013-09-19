@echo off
set PATH=C:\MinGW\bin;%PATH%
@echo Creating qScreenshot Project
svn checkout http://qscreenshot.googlecode.com/svn/trunk/ qscreenshot
cd qscreenshot\qScreenshot
@echo Start time:  %TIME%>..\..\timestamps.log
call qconf.cmd
configure
mingw32-make -f makefile
@echo Finish time: %TIME%>>..\..\timestamps.log
move /Y src\release\qscreenshot.exe ..\..\qscreenshot.exe
cd ..\..
pause
@echo Archiving qScreenshot Nightly Build
call 7z a -mx9 "qscreenshot-r120-win32.7z" "qscreenshot.exe"
@echo Completed
pause
@echo Uploading archived qScreenshot Nightly Build to Google Code
call googlecode_upload.py -p "qscreenshot" -s "qScreenshot Nightly Build || Qt 4.8.5 || For test only!" -l "NightlyBuild,Windows,Archive" "qscreenshot-r110-win32.zip"
@echo Completed
pause & pause
