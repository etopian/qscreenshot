@echo off
@echo Uploading qScreenshot Windows Installer to Google Code
call googlecode_upload.py -p "qscreenshot" -s "qScreenshot Windows Installer || Qt 4.8.0" -l "Featured,Windows,Installer" "setup\qscreenshot-0.6-win32-setup.exe"
@echo Completed
pause & pause
