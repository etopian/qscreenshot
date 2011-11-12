@echo off
set PATH=C:\QtSDK\Desktop\Qt\4.7.4\mingw\bin;%PATH%
@echo Creating qScreenshot Project
svn checkout http://qscreenshot.googlecode.com/svn/trunk/ qscreenshot
cd qscreenshot\qScreenshot
@echo Start time:  %TIME%>..\..\timestamps.log
configure.exe
mingw32-make -f makefile
@echo Finish time: %TIME%>>..\..\timestamps.log
move /Y src\release\qscreenshot.exe ..\..\qscreenshot.exe
cd ..\..
pause
@echo Archiving qScreenshot Nightly Build
call 7z a -tzip -scsDOS -mx9 qscreenshot-r51-win32.zip "qscreenshot.exe"
@echo Completed
@echo Uploading archived qScreenshot Nightly Build to Google Code
call googlecode_upload.py -p "qscreenshot" -s "qScreenshot Nightly Build || Qt 4.7.4 || For test only!" -l "NightlyBuild,Windows,Archive" "qscreenshot-r92-win32.zip"
@echo Completed
pause & pause
